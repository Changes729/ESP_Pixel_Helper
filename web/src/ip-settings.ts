import { BasicObject } from "./object";

export interface IPSettingField {
  static_ip: string;
  gateway: string;
  mask: string;
}

export class IPSetting extends BasicObject {
  ip: HTMLInputElement;
  gateway: HTMLInputElement;
  mask: HTMLInputElement;
  title: string;

  constructor(name: string, field: IPSettingField, parent?: BasicObject) {
    super(parent);

    this.title = name;
    this.ip = document.createElement("input");
    this.gateway = document.createElement("input");
    this.mask = document.createElement("input");

    this.ip.pattern = "^([0-9]{1,3}.){3}[0-9]{1,3}$";
    this.gateway.pattern = "^([0-9]{1,3}.){3}[0-9]{1,3}$";
    this.mask.pattern = "^([0-9]{1,3}.){3}[0-9]{1,3}$";

    this.ip.value = field.static_ip;
    this.gateway.value = field.gateway;
    this.mask.value = field.mask;
  }

  update(value: IPSettingField) {
    this.ip.value = value.static_ip;
    this.gateway.value = value.gateway;
    this.mask.value = value.mask;
  }

  value(): IPSettingField {
    return {
      static_ip: this.ip.value,
      gateway: this.gateway.value,
      mask: this.mask.value,
    };
  }

  render(): Node {
    let root = document.createDocumentFragment();
    let title = document.createElement("label");
    let div = document.createElement("div");
    let label_static = document.createElement("label");
    let label_gateway = document.createElement("label");
    let label_mask = document.createElement("label");

    title.innerText = this.title;
    label_static.innerText = "Static IP:";
    label_gateway.innerText = "Gateway:";
    label_mask.innerText = "Mask:";

    root.append(title);
    var sequence = [
      label_static,
      this.ip,
      label_gateway,
      this.gateway,
      label_mask,
      this.mask,
    ];
    for (var child of sequence) {
      div.appendChild(child);
    }
    root.append(div);

    return root;
  }
}
