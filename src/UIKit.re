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

let _CGRectContainsPoint = (rect, point) =>
  point.x > rect.origin.x
  && point.x < rect.origin.x
  +. rect.size.width
  && point.y > rect.origin.y
  && point.x < rect.origin.x
  +. rect.size.height;

module NSString = {
  type nsstring;
  type t = objcT(nsstring);
  external newWithString : string => t = "NSString_newWithString";
  external _UTF8String : t => string = "NSString_UTF8String";
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
  type t;
  let _new: unit => t;
  let newWithFrame: _CGRect => t;
  let frame: t => _CGRect;
  let setFrame: (t, _CGRect) => unit;
  let layoutSubviews: t => unit;
  let sizeToFit: t => unit;
  let sizeThatFits: (t, _CGSize) => _CGSize;
  let setLayoutSubviews: (t, unit => unit) => unit;
  let setTouchesBegan: (t, _CGPoint => unit) => unit;
  let setTouchesMoved: (t, _CGPoint => unit) => unit;
  let setTouchesEnded: (t, _CGPoint => unit) => unit;
  let setBackgroundColor: (t, UIColor.t) => unit;
  let addSubview: (t, t) => unit;
  let setClipsToBounds: (t, bool) => unit;
} = {
  type uiview;
  type t = {
    objc: objcT(uiview),
    mutable layoutSubviews: option(unit => unit),
    mutable touchesBegan: option(_CGPoint => unit),
    mutable touchesMoved: option(_CGPoint => unit),
    mutable touchesEnded: option(_CGPoint => unit)
  };
  /* [@c.new] external _new : unit => t = ""; */
  external _new : unit => t = "UIView_new";
  /* [@c.new] external newWithFrame : _CGRect => t = ""; */
  external newWithFrame : _CGRect => t = "UIView_newWithFrame";
  /* [@c.property] external frame : _CGRect = ""; */
  external frame : t => _CGRect = "UIView_frame";
  external setFrame : (t, _CGRect) => unit = "UIView_setFrame";
  external layoutSubviews : t => unit = "UIView_layoutSubviews";
  external sizeToFit : t => unit = "UIView_sizeToFit";
  external sizeThatFits : (t, _CGSize) => _CGSize = "UIView_sizeThatFits";
  /*
     @Problem
     This is annoying, I had to subclass UIView to keep track of the closure written in Reason that
     registered to be called on layoutSubviews. I'd be nice to have a more generalized solution that
     avoids as much memory duplicated as possible.

     This function returns the callback to make sure the OCaml GC does not deallocate the closure, because we attach it to the uiview.
   */
  external setLayoutSubviews : (t, unit => unit) => unit =
    "UIView_setLayoutSubviews";
  external setTouchesBegan : (t, _CGPoint => unit) => unit =
    "UIView_setTouchesBegan";
  external setTouchesMoved : (t, _CGPoint => unit) => unit =
    "UIView_setTouchesMoved";
  external setTouchesEnded : (t, _CGPoint => unit) => unit =
    "UIView_setTouchesEnded";
  /* [@c.property] external backgroundColor : UIColor.t = ""; */
  external backgroundColor : t => UIColor.t = "UIView_backgroundColor";
  external setBackgroundColor : (t, UIColor.t) => unit =
    "UIView_setBackgroundColor";
  external addSubview : (t, t) => unit = "UIView_addSubview";
  external setClipsToBounds : (t, bool) => unit = "UIView_setClipsToBounds";
};

module UILabel = {
  include UIView;
  type uilabel;
  type t2 = objcT(uilabel);
  external _new : unit => t = "UILabel_new";
  external text : t => NSString.t = "UILabel_text";
  external setText : (t, NSString.t) => unit = "UILabel_setText";
};
