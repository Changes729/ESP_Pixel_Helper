import { BasicObject } from "./object";

export interface debugField {
  enable: false | true;
  ip: string;
  port: string;
}

export class DebugSettings extends BasicObject {
  enable: HTMLInputElement;
  ip: HTMLInputElement;
  port: HTMLInputElement;

  constructor(parent?: BasicObject) {
    super(parent);
    this.enable = document.createElement("input");
    this.ip = document.createElement("input");
    this.port = document.createElement("input");

    this.enable.type = "checkbox";
    this.ip.pattern = "^([0-9]{1,3}.){3}[0-9]{1,3}$";
    this.port.pattern = "^([0-9]).$";
  }

  update(value: debugField) {
    this.enable.checked = value.enable;
    this.ip.value = value.ip;
    this.port.value = value.port;
  }

  value(): debugField {
    return {
      enable: this.enable.checked,
      ip: this.ip.value,
      port: this.port.value,
    };
  }

  render(): HTMLDivElement {
    let div = document.createElement("div");
    var label_enable = document.createElement("label");
    var label_ip = document.createElement("label");
    var label_port = document.createElement("label");

    label_enable.innerText = "Enable:";
    label_ip.innerText = "Box IP:";
    label_port.innerText = "port:";

    var sequence = [
      label_enable,
      this.enable,
      label_ip,
      this.ip,
      label_port,
      this.port,
    ];
    for (var dom of sequence) {
      div.appendChild(dom);
    }

    return div;
  }
}
