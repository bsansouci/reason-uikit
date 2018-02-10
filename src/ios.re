type objcT('a);

external isNil : objcT('a) => bool = "isNil";

type _CGSize = {
  width: float,
  height: float
};

type _CGPoint = {
  x: float,
  y: float
};

type _CGRect = {
  origin: _CGPoint,
  size: _CGSize
};

let _CGRectMake = (x, y, width, height) => {
  origin: {
    x,
    y
  },
  size: {
    width,
    height
  }
};

/* [@c.class] */
module UIColor = {
  type uicolor;
  type t = objcT(uicolor);
  /* [@c.static] external redColor : unit => t = ""; */
  external redColor : unit => t = "UIColor_redColor";
  /* [@c.static] external greenColor : unit => t = ""; */
  external greenColor : unit => t = "UIColor_greenColor";
};

module rec UIViewController: {
  type uiviewcontroller;
  type t;
  let setView: (t, UIView.t) => unit;
} = {
  type uiviewcontroller;
  type t = objcT(uiviewcontroller);
  external setView : (t, UIView.t) => unit = "UIViewController_setView";
}
and UIView: {
  type uiview;
  type cb;
  type t;
  let _new: unit => t;
  let newWithFrame: _CGRect => t;
  let frame: t => _CGRect;
  let setFrame: (t, _CGRect) => unit;
  let layoutSubviews: t => unit;
  let setLayoutSubviews: (t, unit => unit) => cb;
  let setBackgroundColor: (t, UIColor.t) => unit;
  let addSubview: (t, t) => unit;
  let setClipsToBounds : (t, bool) => unit;
} = {
  type uiview;
  type cb;
  type t = objcT(uiview);
  /* [@c.new] external _new : unit => t = ""; */
  external _new : unit => t = "UIView_new";
  /* [@c.new] external newWithFrame : _CGRect => t = ""; */
  external newWithFrame : _CGRect => t = "UIView_newWithFrame";
  /* [@c.property] external frame : _CGRect = ""; */
  external frame : t => _CGRect = "UIView_frame";
  external setFrame : (t, _CGRect) => unit = "UIView_setFrame";
  external layoutSubviews : t => unit = "UIView_layoutSubviews";
  /*
     @Problem
     This is annoying, I had to subclass UIView to keep track of the closure written in Reason that
     registered to be called on layoutSubviews. I'd be nice to have a more generalized solution that
     avoids as much memory duplicated as possible.

     This function returns the callback to make sure the OCaml GC does not deallocate the closure, because we attach it to the uiview.
   */
  external setLayoutSubviews : (t, unit => unit) => cb =
    "UIView_setLayoutSubviews";
  /* [@c.property] external backgroundColor : UIColor.t = ""; */
  external setBackgroundColor : (t, UIColor.t) => unit =
    "UIView_setBackgroundColor";
  external addSubview : (t, t) => unit = "UIView_addSubview";
  external setClipsToBounds : (t, bool) => unit = "UIView_setClipsToBounds";
};

let main = mainController => {
  let v = UIView._new();
  let _cb =
    UIView.setLayoutSubviews(
      v,
      () => {
        let subview = UIView.newWithFrame(_CGRectMake(100., 100., 100., 100.));
        UIView.setBackgroundColor(subview, UIColor.redColor());
        UIView.setClipsToBounds(subview, true);
        let subview2 =
          UIView.newWithFrame(_CGRectMake(50., 50., 100., 100.));
        UIView.setBackgroundColor(subview2, UIColor.greenColor());
        UIView.addSubview(subview, subview2);
        UIView.addSubview(v, subview);
      }
    );
  UIViewController.setView(mainController, v);
  print_endline("Hello Sailor 1");
};

Callback.register("main", main);
/* Examples of C ppx attributes that would help make this viable */
/*
  [@c.class]
  module UIView {
    type uiview;
    type t = objcT(uiview);
    [@c.new] external _new : unit => t = "";
    [@c.new] external newWithFrame : _CGRect => t = "";
    [@c.property] external frame : _CGRect = "";
    [@c.property] external backgroundColor : UIColor.t = "";
    [@c.method] external layoutSubviews : unit => unit = "";
  };
 */
/*

 */
/* Below is an example of a slightly more palatable syntax. */
/*
  [@c.main]
  let main = mainController => {
    let v = [UIView new];
    [UIview setLayoutSubviews:() => {
      let subview = [UIView newWithFrame:_CGRectMake(100., 100., 100., 100.)];
      [subview setBackgroundColor:[UIColor redColor]];
      [v addSubview:subview];
    }]

    [mainController setView:v];
  };
 */
