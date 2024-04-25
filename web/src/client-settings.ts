import { BasicObject } from "./object";

export interface ClientField {
  ip: string;
  port: string;
}

export class ClientSettings extends BasicObject {
  ip: HTMLInputElement;
  port: HTMLInputElement;

  constructor(field: ClientField, parent?: BasicObject) {
    super(parent);
    this.ip = document.createElement("input");
    this.port = document.createElement("input");

    this.ip.value = field.ip;
    this.port.value = field.port;

    this.ip.pattern = "^([0-9]{1,3}.){3}[0-9]{1,3}$";
    this.port.pattern = "^([0-9]).$";
  }

  update(value: ClientField) {
    this.ip.value = value.ip;
    this.port.value = value.port;
  }

  value(): ClientField {
    return { ip: this.ip.value, port: this.port.value };
  }

  render(): HTMLDivElement {
    let div = document.createElement("div");
    var label_ip = document.createElement("label");
    var label_port = document.createElement("label");

    label_ip.innerText = "Box IP:";
    label_port.innerText = "port:";

    var sequence = [label_ip, this.ip, label_port, this.port];
    for (var dom of sequence) {
      div.appendChild(dom);
    }

    return div;
  }
}
