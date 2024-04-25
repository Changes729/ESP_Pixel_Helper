interface IUpdater {
  _onUpdate(): void;
}

export abstract class BasicObject implements IUpdater {
  constructor(props?: BasicObject) {
    this._parent = props;
  }

  abstract render(): Node;

  _onUpdate(): void {
    if (this.parent == undefined) {
      this.render();
    } else {
      this.parent._onUpdate;
    }
  }

  _parent?: BasicObject;

  get parent(): BasicObject | undefined {
    return this._parent;
  }

  set parent(p: BasicObject) {
    this._parent = p;
  }
}
