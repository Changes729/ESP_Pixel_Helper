import { BasicObject } from "./object";

export interface ClientField {
  ip: string;
  port: string;
  touch: string;
  release: string;
}

export class ClientSettings extends BasicObject {
  ip: HTMLInputElement;
  port: HTMLInputElement;
  touch_threadsholds: HTMLInputElement;
  release_threadsholds: HTMLInputElement;

  constructor(parent?: BasicObject) {
    super(parent);
    this.ip = document.createElement("input");
    this.port = document.createElement("input");
    this.touch_threadsholds = document.createElement("input");
    this.release_threadsholds = document.createElement("input");

    this.ip.pattern = "^([0-9]{1,3}.){3}[0-9]{1,3}$";
    this.port.pattern = "^([0-9]).$";
  }

  update(value: ClientField) {
    this.ip.value = value.ip;
    this.port.value = value.port;
    this.touch_threadsholds.value = value.touch;
    this.release_threadsholds.value = value.release;
  }

  value(): ClientField {
    return {
      ip: this.ip.value,
      port: this.port.value,
      touch: this.touch_threadsholds.value,
      release: this.release_threadsholds.value,
    };
  }

  render(): Node {
    let root = document.createDocumentFragment();
    let div_ip = document.createElement("div");
    var label_ip = document.createElement("label");
    var label_port = document.createElement("label");

    label_ip.innerText = "Box IP:";
    label_port.innerText = "port:";

    var sequence = [label_ip, this.ip, label_port, this.port];
    for (var dom of sequence) {
      div_ip.appendChild(dom);
    }

    let div_touch = document.createElement("div");
    var label_touch = document.createElement("label");
    var label_release = document.createElement("label");

    label_touch.innerText = "Touch:";
    label_release.innerText = "Release:";

    var sequence = [
      label_touch,
      this.touch_threadsholds,
      label_release,
      this.release_threadsholds,
    ];
    for (var dom of sequence) {
      div_touch.appendChild(dom);
    }

    root.appendChild(div_ip);
    root.appendChild(div_touch);

    return root;
  }
}
