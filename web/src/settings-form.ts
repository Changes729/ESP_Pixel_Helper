import { ClientSettings } from "./client-settings";
import { DebugSettings } from "./debug-settings";
import { IPSetting, IPSettingField } from "./ip-settings";
import { BasicObject } from "./object";
import { WiFiSettings, wifi_setting } from "./wifi-settings";

abstract class fieldset extends BasicObject {
  legend: string;

  constructor(legend: string, props?: BasicObject) {
    super(props);

    this.legend = legend;
  }

  async save() {}

  render(): HTMLFieldSetElement {
    let fieldset = document.createElement("fieldset") as HTMLFieldSetElement;
    fieldset.innerHTML = `<legend>${this.legend}</legend>`;
    return fieldset;
  }
}

class WiFiSettingFieldset extends fieldset {
  settings: WiFiSettings[] = [];

  constructor(legend: string, settings: wifi_setting[], props?: BasicObject) {
    super(legend, props);

    for (var setting of settings) {
      this.settings.push(new WiFiSettings(setting, this));
    }

    this.reload();
  }

  async reload() {
    console.log("async reload wifi-settings");

    fetch("/api/wpa_supplicant/info")
      .then((response) => response.json())
      .then((json) => {
        console.log(json);
        if (json.metas.length > 0) {
          json.metas.map((value, index) => {
            if (index < json.max_info) {
              this.settings[index].update(value);
            }
          });
        }
      })
      .catch((e) => {
        console.log("exception: ", e);
      });
  }

  async save() {
    console.log("async save wifi-settings");

    var body: wifi_setting[] = [];
    for (var setting of this.settings) {
      body.push(setting.value());
    }

    var xhr = new XMLHttpRequest();
    xhr.open("POST", "/api/wpa_supplicant/info", true);
    xhr.setRequestHeader("Content-Type", "application/json");
    xhr.send(
      JSON.stringify({
        metas: body,
      })
    );
  }

  render(): HTMLFieldSetElement {
    let fieldset = super.render();
    for (let setting of this.settings) {
      fieldset.appendChild(setting.render());
    }

    if (0) {
      fieldset.innerHTML += `<div id="wifi-settings"></div>
      <button type="button" id="add-wifi-settings"> + </button>`;
    }
    return fieldset;
  }
}

class IPSettingFieldset extends fieldset {
  settings: IPSetting[] = [];

  constructor(legend: string, parent?: BasicObject) {
    super(legend, parent);

    this.settings.push(
      new IPSetting("eth", { static_ip: "", gateway: "", mask: "" }, this),
      new IPSetting("wlan", { static_ip: "", gateway: "", mask: "" }, this)
    );

    this.reload();
  }

  async reload() {
    fetch("/api/dhcpcd/eth")
      .then((response) => response.json())
      .then((json) => {
        console.log(json);
        this.settings[0].update(json);
      })
      .catch((e) => {
        console.log("exception: ", e);
      });

    fetch("/api/dhcpcd/wlan")
      .then((response) => response.json())
      .then((json) => {
        console.log(json);
        this.settings[1].update(json);
      })
      .catch((e) => {
        console.log("exception: ", e);
      });
  }

  async save() {
    console.log("async save ip-settings");

    var xhr = new XMLHttpRequest();
    xhr.open("POST", "/api/dhcpcd/eth", true);
    xhr.setRequestHeader("Content-Type", "application/json");
    xhr.send(JSON.stringify(this.settings[0].value()));

    xhr = new XMLHttpRequest();
    xhr.open("POST", "/api/dhcpcd/wlan", true);
    xhr.setRequestHeader("Content-Type", "application/json");
    xhr.send(JSON.stringify(this.settings[1].value()));
  }

  render(): HTMLFieldSetElement {
    let fieldset = super.render();
    var div = document.createElement("div") as HTMLDivElement;
    div.style["display"] = "grid";
    div.style["grid-template-columns"] = "auto 1fr";
    div.style["column-gap"] = "10px";
    div.id = "ip-settings";

    for (var element of this.settings) {
      div.appendChild(element.render());
    }
    fieldset.appendChild(div);
    return fieldset;
  }
}

class APPSettingFieldset extends fieldset {
  client: ClientSettings;

  constructor(legend: string, parent?: BasicObject) {
    super(legend, parent);

    this.client = new ClientSettings(this);

    this.reload();
  }

  async reload() {
    fetch("/api/box/info")
      .then((response) => response.json())
      .then((json) => {
        console.log(json);
        this.client.update(json);
      })
      .catch((e) => {
        console.log("exception: ", e);
      });
  }

  async save() {
    console.log("async save app-settings");

    var xhr = new XMLHttpRequest();
    xhr.open("POST", "/api/box/info", true);
    xhr.setRequestHeader("Content-Type", "application/json");
    xhr.send(JSON.stringify(this.client.value()));
  }

  render(): HTMLFieldSetElement {
    let fieldset = super.render();
    fieldset.appendChild(this.client.render());
    return fieldset;
  }
}

class DebugSettingFieldset extends fieldset {
  setting: DebugSettings;

  constructor(legend: string, parent?: BasicObject) {
    super(legend, parent);

    this.setting = new DebugSettings(this);

    this.reload();
  }

  async reload() {
    fetch("/api/app/info")
      .then((response) => response.json())
      .then((json) => {
        console.log(json);
        this.setting.update(json);
      })
      .catch((e) => {
        console.log("exception: ", e);
      });
  }

  async save() {
    console.log("async save app-settings");

    var xhr = new XMLHttpRequest();
    xhr.open("POST", "/api/app/info", true);
    xhr.setRequestHeader("Content-Type", "application/json");
    xhr.send(JSON.stringify(this.setting.value()));
  }

  render(): HTMLFieldSetElement {
    let fieldset = super.render();
    fieldset.appendChild(this.setting.render());
    return fieldset;
  }
}

const PAGE = {
  WIFI_SETTING: 0,
  IP_SETTING: 1,
  APP_SETTING: 2,
  DEBUG_SETTING: 3,
};

type Ifieldset = {
  [key: number]: fieldset;
};

export class Container extends BasicObject {
  fieldsets: Ifieldset = {};
  button: HTMLButtonElement;

  constructor(parent?: BasicObject) {
    super(parent);
    this.fieldsets[PAGE.WIFI_SETTING] = new WiFiSettingFieldset(
      "WiFi Settings",
      [
        { ssid: "", passwd: "" },
        { ssid: "", passwd: "" },
        { ssid: "", passwd: "" },
      ],
      this
    );

    this.fieldsets[PAGE.IP_SETTING] = new IPSettingFieldset(
      "IP Settings",
      this
    );

    this.fieldsets[PAGE.APP_SETTING] = new APPSettingFieldset(
      "APP Settings",
      this
    );

    this.fieldsets[PAGE.DEBUG_SETTING] = new DebugSettingFieldset(
      "Debug Settings",
      this
    );

    //FIXME:
    {
      this.button = document.createElement("button");
      this.button.className = "primary";
      this.button.id = "savebtn";
      this.button.type = "button";
      this.button.innerText = "SAVE";
      this.button.onclick = () => {
        this.on_save();
      };
    }
  }

  render() {
    let form = document.createElement("form") as HTMLFormElement;
    // TODO: form.removeChild();

    console.log(this.fieldsets);
    for (var key in PAGE) {
      form.appendChild(this.fieldsets[PAGE[key]].render());
    }
    form.appendChild(this.button);

    return form;
  }

  on_save() {
    console.log("button was clicked!");
    for (var key in PAGE) {
      this.fieldsets[PAGE[key]].save();
    }
  }
}
