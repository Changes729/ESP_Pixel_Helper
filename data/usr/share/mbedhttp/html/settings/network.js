var MAX_WIFI_ELEMENT = 3;

function createElementFromHTML(htmlString) {
  var div = document.createElement("div");
  div.innerHTML = htmlString.trim();

  // Change this to div.childNodes to support multiple top-level nodes.
  return div.firstChild;
}

function wifi_settings_item(ssid, passwd) {
  return createElementFromHTML(`<div class="WiFiUnit">
  <label for="ssid">SSID</label>
  <input value="${ssid}" class="ssid" placeholder="SSID" />
  <label for="password">Password</label>
  <input type="password" value="${passwd}" class="password" placeholder="PASSWORD" />
  </div>`);
}

function ip_settings_title(place_holder) {
  return createElementFromHTML(`<label style="width:">${place_holder}</label>`);
}

function ip_settings_form(id) {
  return createElementFromHTML(`<div id=${id}>
      <label for="ip">Static IP:</label>
      <input class="ip" required pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$">
      <label for="gateway">Gateway:</label>
      <input class="gateway" required pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$">
      <label for="mask">Mask:</label>
      <input class="mask" required pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$">
    </div>`);
}

window.addEventListener("load", (event) => {
  console.log("on windows load.");
  var ip_settings = document.getElementById("ip-settings");

  ip_settings.appendChild(ip_settings_title("eth"));
  ip_settings.appendChild(ip_settings_form("eth0"));
  ip_settings.appendChild(ip_settings_title("wlan"));
  ip_settings.appendChild(ip_settings_form("wlan0"));

  reload_wifi_settings();
  reload_eth_settings();
  reload_wlan_settings();

  document.getElementById("savebtn").onclick = on_save;
  document.getElementById("add-wifi-settings").onclick = add_wifi_settings;
});

function on_wifi_settings_update(mutationsList) {
  var wifi_setting = document.getElementById("wifi-settings");
  var button = document.getElementById("add-wifi-settings");
  for (let mutation of mutationsList) {
    if (mutation.type === "childList") {
      if (wifi_setting.childElementCount >= MAX_WIFI_ELEMENT) {
        button.style.display = "none";
      }
    }
  }
}

function add_wifi_settings() {
  var wifi_setting = document.getElementById("wifi-settings");
  if (wifi_setting.childElementCount < MAX_WIFI_ELEMENT) {
    wifi_setting.appendChild(wifi_settings_item("", ""));
  }
}

function on_save() {
  console.log("button was clicked!");

  body = [];
  var wifi_setting = document.getElementById("wifi-settings");
  var units = wifi_setting.getElementsByClassName("WiFiUnit");
  for (var i = 0; i < wifi_setting.childElementCount; ++i) {
    var ssid = units[i].getElementsByClassName("ssid")[0].value;
    var passwd = units[i].getElementsByClassName("password")[0].value;
    body.push({ ssid: ssid, passwd: passwd });
  }

  var xhr = new XMLHttpRequest();
  xhr.open("POST", "/api/wpa_supplicant/info", true);
  xhr.setRequestHeader("Content-Type", "application/json");
  xhr.send(
    JSON.stringify({
      metas: body,
    })
  );

  var eth = document.getElementById("eth0");
  var input_ip = eth.getElementsByClassName("ip")[0];
  var input_gateway = eth.getElementsByClassName("gateway")[0];
  var input_mask = eth.getElementsByClassName("mask")[0];
  xhr = new XMLHttpRequest();
  xhr.open("POST", "/api/dhcpcd/eth", true);
  xhr.setRequestHeader("Content-Type", "application/json");
  xhr.send(
    JSON.stringify({
      static_ip: input_ip.value,
      gateway: input_gateway.value,
      mask: input_mask.value,
    })
  );

  var wlan = document.getElementById("wlan0");
  input_ip = wlan.getElementsByClassName("ip")[0];
  input_gateway = wlan.getElementsByClassName("gateway")[0];
  input_mask = wlan.getElementsByClassName("mask")[0];
  xhr = new XMLHttpRequest();
  xhr.open("POST", "/api/dhcpcd/wlan", true);
  xhr.setRequestHeader("Content-Type", "application/json");
  xhr.send(
    JSON.stringify({
      static_ip: input_ip.value,
      gateway: input_gateway.value,
      mask: input_mask.value,
    })
  );
}

function reload_wifi_settings() {
  var wifi_settings = document.getElementById("wifi-settings");
  const observer = new MutationObserver(on_wifi_settings_update);
  const config = { attributes: true, childList: true, subtree: true };
  observer.observe(wifi_settings, config);

  fetch("/api/wpa_supplicant/info")
    .then((response) => response.json())
    .then((json) => {
      MAX_WIFI_ELEMENT = json.max_info;
      if (json.metas.length > 0) {
        for (var meta of json.metas) {
          wifi_settings.appendChild(wifi_settings_item(meta.ssid, meta.passwd));
        }
      } else {
        wifi_settings.appendChild(wifi_settings_item("", ""));
      }
    })
    .catch(function () {
      wifi_settings.appendChild(wifi_settings_item("", ""));
    });
}

function reload_eth_settings() {
  var eth = document.getElementById("eth0");
  var input_ip = eth.getElementsByClassName("ip")[0];
  var input_gateway = eth.getElementsByClassName("gateway")[0];
  var input_mask = eth.getElementsByClassName("mask")[0];

  fetch("/api/dhcpcd/eth")
    .then((response) => response.json())
    .then((json) => {
      input_ip.value = json.static_ip;
      input_gateway.value = json.gateway;
      input_mask.value = json.mask;
    })
    .catch();
}

function reload_wlan_settings() {
  var wlan = document.getElementById("wlan0");
  var input_ip = wlan.getElementsByClassName("ip")[0];
  var input_gateway = wlan.getElementsByClassName("gateway")[0];
  var input_mask = wlan.getElementsByClassName("mask")[0];

  fetch("/api/dhcpcd/wlan")
    .then((response) => response.json())
    .then((json) => {
      input_ip.value = json.static_ip;
      input_gateway.value = json.gateway;
      input_mask.value = json.mask;
    })
    .catch();
}
