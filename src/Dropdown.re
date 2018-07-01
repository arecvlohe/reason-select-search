[%bs.raw {|require('./Dropdown.css')|}];

type retainedProps = {countries: Models.countries};

type state = {
  country: Models.country,
  isOpen: bool,
  focusTabIndex: int,
  divRef: ref(option(Dom.element))
};

let setDivRef = (theRef, {ReasonReact.state}) =>
  state.divRef := Js.Nullable.toOption(theRef);

type action =
  | ToggleDropdown
  | SelectCountry(Models.country)
  | FocusNextTabIndex
  | FocusPrevTabIndex
  | SelectCurrentTabIndex
  | CloseDropdown
  | OpenDropdown
  | NoOp;

let component = ReasonReact.reducerComponentWithRetainedProps("Dropdown");

let make =
    (~initialCountryValue, ~countries, ~handleChange, ~results, _children) => {
  ...component,
  initialState: () => {
    country: {
      label: "",
      value: ""
    },
    isOpen: false,
    focusTabIndex: (-1),
    divRef: ref(None)
  },
  retainedProps: {
    countries: countries
  },
  didMount: self =>
    Webapi.Dom.document
    |> Webapi.Dom.Document.addKeyDownEventListener(event => {
         let code = Webapi.Dom.KeyboardEvent.code(event);
         switch code {
         | "BracketLeft" =>
           self.send(OpenDropdown);
           switch self.state.divRef^ {
           | None => ()
           | Some(r) => ReactDOMRe.domElementToObj(r)##focus()
           };
         | "BracketRight" => self.send(CloseDropdown)
         | _ => ()
         };
       }),
  didUpdate: ({oldSelf, newSelf}) => {
    let prevLength = oldSelf.retainedProps.countries |> Array.length;
    let nextLength = newSelf.retainedProps.countries |> Array.length;
    if (prevLength !== nextLength) {
      let country =
        newSelf.retainedProps.countries
        |> Js.Array.find((country: Models.country) =>
             country.value === initialCountryValue
           );
      switch country {
      | None => ()
      | Some(country) => newSelf.send(SelectCountry(country))
      };
    } else {
      ();
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
        (_self => handleChange(country))
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
      let selectedCountry: Models.country =
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
    | OpenDropdown => ReasonReact.Update({...state, isOpen: true})
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
                  (index, country: Models.country) =>
                    <div
                      key=country.value
                      id=country.value
                      className="Dropdown-country"
                      onClick=(_event => self.send(SelectCountry(country)))
                      tabIndex=index>
                      (Utils.text(country.label))
                    </div>,
                  Array.length(results) < 1 ? countries : results
                )
              )
            )
          </div> :
          ReasonReact.null
      )
    </div>
};
