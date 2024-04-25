import { BasicObject } from "./object";

export interface wifi_setting {
  ssid: string;
  passwd: string;
}

export class WiFiSettings extends BasicObject {
  ssid: HTMLInputElement;
  passwd: HTMLInputElement;

  constructor(setting: wifi_setting, parent?: BasicObject) {
    super(parent);

    this.ssid = document.createElement("input") as HTMLInputElement;
    this.passwd = document.createElement("input") as HTMLInputElement;

    this.ssid.value = setting.ssid;
    this.ssid.placeholder = "SSID";

    this.passwd.value = setting.passwd;
    this.passwd.placeholder = "PASSWORD";
  }

  update(value: wifi_setting): void {
    this.ssid.value = value.ssid;
    this.passwd.value = value.passwd;
  }

  value(): wifi_setting {
    return { ssid: this.ssid.value, passwd: this.passwd.value };
  }

  render(): HTMLElement {
    let div = document.createElement("div");
    let label_ssid = document.createElement("label");
    let label_passwd = document.createElement("label");

    label_ssid.innerText = "SSID";
    label_passwd.innerText = "Passwd";
    div.appendChild(label_ssid);
    div.appendChild(this.ssid);
    div.appendChild(label_passwd);
    div.appendChild(this.passwd);

    return div;
  }
}
