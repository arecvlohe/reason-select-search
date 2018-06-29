[%bs.raw {|require('./index.css')|}];

[%bs.raw {|require('../node_modules/flag-icon-css/css/flag-icon.min.css')|}];

[@bs.module "./registerServiceWorker"]
external register_service_worker : unit => unit = "default";

ReactDOMRe.renderToElementWithId(
  <App />,
  "root",
);

register_service_worker();
