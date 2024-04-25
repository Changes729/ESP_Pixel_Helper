import { Container } from "./settings-form";

let container = new Container();

window.addEventListener("load", (event) => {
  console.log("on windows load.");

  const app = document.getElementById("app");
  app?.append(container.render());
});
