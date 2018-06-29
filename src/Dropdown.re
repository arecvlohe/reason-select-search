[%bs.raw {|require('./Dropdown.css')|}];

type state = {
  countries: Countries.countries,
  country: Countries.country,
  isOpen: bool,
  focusTabIndex: int,
  divRef: ref(option(Dom.element))
};

let setDivRef = (theRef, {ReasonReact.state}) =>
  state.divRef := Js.Nullable.toOption(theRef);

type action =
  | ToggleDropdown
  | SelectCountry(Countries.country)
  | FocusNextTabIndex
  | FocusPrevTabIndex
  | SelectCurrentTabIndex
  | CloseDropdown
  | OpenDropdown
  | NoOp;

let component = ReasonReact.reducerComponent("Dropdown");

let make = (~initialCountryValue, ~handleChange, ~results, _children) => {
  ...component,
  initialState: () => {
    countries: Countries.countriesArray,
    country: {
      label: "",
      value: ""
    },
    isOpen: false,
    focusTabIndex: (-1),
    divRef: ref(None)
  },
  didMount: self => {

    Webapi.Dom.document |> Webapi.Dom.Document.addKeyDownEventListener(event => {
      let code = Webapi.Dom.KeyboardEvent.code(event);
      switch (code) {
      | "KeyO" => {
        self.send(OpenDropdown);
        switch self.state.divRef^ {
        | None => ()
        | Some(r) => ReactDOMRe.domElementToObj(r)##focus()
        };
      }
      | "KeyC" => self.send(CloseDropdown);
      | _ => ()
      }
    });

    let country =
      self.state.countries
      |> Js.Array.find((country: Countries.country) =>
           country.value === initialCountryValue
         );
    switch country {
    | None => ()
    | Some(country) => self.send(SelectCountry(country))
    };
  },
  reducer: (action: action, state: state) =>
    switch action {
    | ToggleDropdown =>
      ReasonReact.UpdateWithSideEffects(
        {...state, isOpen: ! state.isOpen},
        (
          self =>
            switch self.state.divRef^ {
            | None => ()
            | Some(r) => ReactDOMRe.domElementToObj(r)##focus()
            }
        )
      )
    | SelectCountry(country) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, country, isOpen: false, focusTabIndex: (-1)},
        (_self => handleChange(country) )
      )
    | FocusNextTabIndex when state.focusTabIndex < Array.length(results) - 1 =>
      ReasonReact.UpdateWithSideEffects(
        {...state, focusTabIndex: state.focusTabIndex + 1},
        (
          self =>
            switch self.state.divRef^ {
            | None => ()
            | Some(r) =>
              let children = ReactDOMRe.domElementToObj(r)##children;
              let child = children[self.state.focusTabIndex];
              child##focus();
            }
        )
      )
    | FocusPrevTabIndex when state.focusTabIndex - 1 > (-1) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, focusTabIndex: state.focusTabIndex - 1},
        (
          self =>
            switch self.state.divRef^ {
            | None => ()
            | Some(r) =>
              let children = ReactDOMRe.domElementToObj(r)##children;
              let child = children[self.state.focusTabIndex];
              child##focus();
            }
        )
      )
    | FocusNextTabIndex when state.focusTabIndex > Array.length(results) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, focusTabIndex: (-1)},
        (self => self.send(FocusNextTabIndex))
      )
    | FocusNextTabIndex => ReasonReact.NoUpdate
    | FocusPrevTabIndex => ReasonReact.NoUpdate
    | SelectCurrentTabIndex =>
      let selectedCountry: Countries.country =
        switch state.divRef^ {
        | None => {label: "", value: ""}
        | Some(r) =>
          let children = ReactDOMRe.domElementToObj(r)##children;
          let child = children[state.focusTabIndex];
          {label: child##innerHTML, value: child##id};
        };
      ReasonReact.UpdateWithSideEffects(
        {
          ...state,
          country: selectedCountry,
          isOpen: false,
          focusTabIndex: (-1)
        },
        (_self => handleChange(selectedCountry))
      );
    | OpenDropdown => ReasonReact.Update({ ...state, isOpen: true })
    | CloseDropdown => ReasonReact.Update({...state, isOpen: false})
    | NoOp => ReasonReact.NoUpdate
    },
  render: self =>
    <div className="Dropdown">
      <span>
        (
          self.state.country.label === "" ?
            Utils.text("Please select a country") :
            Utils.text(self.state.country.label)
        )
      </span>
      <span
        className="Dropdown-caret"
        onClick=(_event => self.send(ToggleDropdown))>
        <i className="fas fa-caret-down" />
      </span>
      (
        self.state.isOpen ?
          <div
            tabIndex=(-1)
            className="Dropdown-countries"
            ref=(self.handle(setDivRef))
            onKeyDown=(
              event => {
                let which = ReactEventRe.Keyboard.which(event);
                switch which {
                | 40 => self.send(FocusNextTabIndex)
                | 38 => self.send(FocusPrevTabIndex)
                | 13 => self.send(SelectCurrentTabIndex)
                | 27 => self.send(CloseDropdown)
                | _ => ()
                };
              }
            )>
            (
              ReasonReact.array(
                Array.mapi(
                  (index, country: Countries.country) =>
                    <div
                      key=country.value
                      id=country.value
                      className="Dropdown-country"
                      onClick=(_event => self.send(SelectCountry(country)))
                      tabIndex=index>
                      (Utils.text(country.label))
                    </div>,
                  Array.length(results) < 1 ? self.state.countries : results
                )
              )
            )
          </div> :
          ReasonReact.null
      )
    </div>
};
