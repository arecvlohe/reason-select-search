[%bs.raw {|require('./SearchInput.css')|}];

type retainedProps= {
  country: Countries.country
}

type state = {
  countries: Countries.countries,
  input: string,
  results: Countries.countries,
  inputRef: ref(option(Dom.element))
};

let setInputRef = (theRef, {ReasonReact.state}) =>
  state.inputRef := Js.Nullable.toOption(theRef);

type action =
  | UpdateInput(string)
  | NoOp;

let component = ReasonReact.reducerComponentWithRetainedProps("SearchInput");

let make = (~handleResults, ~country, _children) => {
  ...component,
  retainedProps: {
    country: country
  },
  initialState: () => {
    countries: Countries.countriesArray,
    input: "",
    results: [||],
    inputRef: ref(None)
  },
  didMount: self => {

    Webapi.Dom.document |> Webapi.Dom.Document.addKeyUpEventListener(event => {
      let code = Webapi.Dom.KeyboardEvent.code(event);
      switch (code) {
      | "KeyS" => {
        switch self.state.inputRef^ {
        | None => ()
        | Some(r) => ReactDOMRe.domElementToObj(r)##focus()
        };
      }
      | _ => ()
      }
    });

  },
  didUpdate: ({oldSelf, newSelf}) => {
    if (oldSelf.retainedProps.country.value !== newSelf.retainedProps.country.value) {
      newSelf.send(UpdateInput(""));
    }
  },
  reducer: (action: action, state: state) =>
    switch action {
    | UpdateInput(input) =>
      let results =
        state.countries
        |> Js.Array.filter((country: Countries.country) =>
             Js.String.startsWith(
               input |> Js.String.toLowerCase,
               country.label |> Js.String.toLowerCase
             )
           );
      ReasonReact.UpdateWithSideEffects(
        {...state, input, results},
        (_self => handleResults(results))
      );
    | NoOp => ReasonReact.NoUpdate
    },
  render: self =>
    <div className="SearchInput">
      <span className="SearchInput-icon">
        <i className="fas fa-search" />
      </span>
      <input
        ref=(self.handle(setInputRef))
        style=(
          String.length(self.state.input) > 0
          && Array.length(self.state.results) > 0 ?
            ReactDOMRe.Style.make(
              ~borderBottomLeftRadius="0",
              ~borderBottomRightRadius="0",
              ()
            ) :
            ReactDOMRe.Style.make()
        )
        className="SearchInput-input"
        value=self.state.input
        placeholder="Search"
        onChange=(
          event => {
            let target =
              event |> ReactEventRe.Form.target |> ReactDOMRe.domElementToObj;
            self.send(UpdateInput(target##value));
          }
        )
      />
      (
        String.length(self.state.input) > 0 ?
          {
            let hasNoResults = Array.length(self.state.results) < 1;
            <div
              className="SearchInput-results"
              style=(
                hasNoResults ?
                  ReactDOMRe.Style.make(
                    ~borderTopLeftRadius="0px",
                    ~borderTopRightRadius="0px",
                    ~borderBottom="0",
                    ()
                  ) :
                  ReactDOMRe.Style.make()
              )>
              (
                ReasonReact.array(
                  self.state.results
                  |> Array.mapi((index, country: Countries.country) => {
                       let {label, value}: Countries.country = country;
                       <div
                         key=country.value
                         id=country.value
                         className="SearchInput-country"
                         tabIndex=index>
                         <span
                           style=(
                             ReactDOMRe.Style.make(~marginRight="10px", ())
                           )
                           className={j|flag-icon flag-icon-$value flag-icon-squared|j}
                         />
                         <span> (Utils.text(label)) </span>
                       </div>;
                     })
                )
              )
            </div>;
          } :
          ReasonReact.null
      )
    </div>
};
