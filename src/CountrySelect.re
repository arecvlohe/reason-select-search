type state = {
  country: Models.country,
  results: Models.countries,
  countries: Models.countries
};

type action =
  | UpdateResults(Models.countries)
  | UpdateCountry(Models.country)
  | UpdateCountries(Models.countries)
  | NoOp;

let component = ReasonReact.reducerComponent("CountrySelect");

let make = (~className, ~country, ~onChange, _children) => {
  ...component,
  initialState: () => {
    country: {
      label: "",
      value: ""
    },
    countries: [||],
    results: [||]
  },
  didMount: self =>
    Js.Promise.(
      Fetch.fetch(Utils.url)
      |> then_(Fetch.Response.text)
      |> then_(text => {
           let array = Json.parseOrRaise(text) |> Decoders.countries;
           self.send(UpdateCountries(array));
           Js.Promise.resolve();
         })
    )
    |> ignore,
  reducer: (action: action, state: state) =>
    switch action {
    | UpdateResults(results) => ReasonReact.Update({...state, results})
    | UpdateCountry(country) => ReasonReact.Update({...state, country})
    | UpdateCountries(countries) => ReasonReact.Update({...state, countries})
    | NoOp => ReasonReact.NoUpdate
    },
  render: self =>
    <div className>
      <Dropdown
        initialCountryValue=country
        countries=self.state.countries
        handleChange=(
          country => {
            onChange(country);
            self.send(UpdateCountry(country));
          }
        )
        results=self.state.results
      />
      <SearchInput
        country=self.state.country
        countries=self.state.countries
        handleResults=(results => self.send(UpdateResults(results)))
      />
    </div>
};
