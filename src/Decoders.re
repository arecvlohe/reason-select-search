let country = (json: Js.Json.t) : Models.country =>
  Json.Decode.{
    label: json |> field("label", string),
    value: json |> field("value", string)
  };

let countries = (json: Js.Json.t) : Models.countries =>
  json |> Json.Decode.array(country);
