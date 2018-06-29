type country = {
  label: string,
  value: string
};

type state = {country: Countries.country, results: Countries.countries};

type action =
  | UpdateResults(Countries.countries)
  | UpdateCountry(Countries.country)
  | NoOp;

let component = ReasonReact.reducerComponent("CountrySelect");

let make = (~className, ~country, ~onChange, _children) => {
  ...component,
  initialState: () => {
    country: { label: "", value: "" },
    results: [||]
  },
  reducer: (action: action, state: state) =>
    switch action {
    | UpdateResults(results) => ReasonReact.Update({...state, results: results})
    | UpdateCountry(country) => ReasonReact.Update({ ...state, country })
    | NoOp => ReasonReact.NoUpdate
    },
  render: self =>
    <div className>
      <Dropdown
        initialCountryValue=country
        handleChange={country => {
          onChange(country);
          self.send(UpdateCountry(country))
        }}
        results=self.state.results
      />
      <SearchInput
        country=self.state.country
        handleResults=(results => self.send(UpdateResults(results)))
      />
    </div>
};
