[%bs.raw {|require('./App.css')|}];

[@bs.module] external logo : string = "./logo.svg";

let component = ReasonReact.statelessComponent("App");

let make = (_children) => {
  ...component,
  render: _self =>
    <div className="App">
      <CountrySelect className="custom-class" country="us" onChange=(country => Js.log(country)) />
    </div>,
};
