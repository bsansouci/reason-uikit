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

type _UIEdgeInsets = {
  bottom: float,
  left: float,
  right: float,
  top: float
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

let _UIEdgeInsetsMake = (top, left, bottom, right) => {
  top,
  left,
  bottom,
  right
};

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
  external whiteColor : unit => t = "UIColor_whiteColor";
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
[@c.class]
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
   [@c.new] external _new : unit => t = "";
  /*external _new : unit => t = "UIView_new";*/
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

[@c.class]
module UILabel = {
  include UIView;
  type uilabel;
  type t2 = objcT(uilabel);
  [@c.new] external _new : unit => t = "";
  external text : t => NSString.t = "UILabel_text";
  [%c.raw
    {|
    void UILabel_setText(value uilabel, value text) {
      CAMLparam2(uilabel, text);
      ((UILabel *)Field(uilabel, 0)).text = (NSString *) text;
      CAMLreturn0;
    }
  |}
  ];
  external setText : (t, NSString.t) => unit = "UILabel_setText";
  [%c.raw
    {|
    CAMLprim value UILabel_text(value uilabel) {
      CAMLparam1(uilabel);
      CAMLreturn((value)((UILabel *)Field(uilabel, 0)).text);
    }
  |}
  ];
};

[@c.class]
module UIImage = {
  type uiimage;
  type t = uiimage;
  [@c.new] external _new : unit => t = "";
  external imageNamed : string => option(t) = "UIImage_imageNamed";
  [%c.raw
    {|
    CAMLprim value UIImage_imageNamed(value name) {
      CAMLparam1(name);
      UIImage *maybeImage = [UIImage imageNamed:[[NSString alloc] initWithUTF8String:String_val(name)]];
      if (!maybeImage) {
        CAMLreturn(Val_none);
      }
      CAMLreturn(Val_some((value) maybeImage));
    }
    |}
  ];
  external resizableImageWithCapInsets : (t, _UIEdgeInsets) => t =
    "UIImage_resizableImageWithCapInsets";
  [%c.raw
    {|
    CAMLprim value UIImage_resizableImageWithCapInsets(value uiimage, value insets) {
      CAMLparam2(uiimage, insets);
      CAMLreturn((value)[(UIImage *)uiimage resizableImageWithCapInsets:
        UIEdgeInsetsMake(Double_field(insets, 3), Double_field(insets, 1), Double_field(insets, 0), Double_field(insets, 2))]);
    }
    |}
  ];
};

[@c.class]
module UIImageView = {
  include UIView;
  type uiimageview;
  type t2 = objcT(uiimageview);
  [@c.new] external _new : unit => t = "";
  [@c.new] external newWithImage : UIImage.t => t = "";
};

module UIScreen = {
  type uiscreen;
  type t = uiscreen;
  external mainScren : unit => t = "UIScreen_mainScreen";
  [%c.raw
    {|
    CAMLprim value UIScreen_mainScreen() {
      CAMLparam0();
      CAMLreturn((value)[UIScreen mainScreen]);
    }
  |}
  ];
  external bounds : t => _CGRect = "UIScreen_bounds";
  [%c.raw
    {|
    CAMLprim value UIScreen_bounds(value screen) {
      CAMLparam1(screen);
      CAMLlocal3(ret, origin, size);

      CGRect r = [((UIScreen *)screen) bounds];

      origin = caml_alloc_small(2, Double_array_tag);
      Double_field(origin, 0) = (double)r.origin.x;
      Double_field(origin, 1) = (double)r.origin.y;

      size = caml_alloc_small(2, Double_array_tag);
      Double_field(size, 0) = (double)r.size.width;
      Double_field(size, 1) = (double)r.size.height;

      ret = caml_alloc_small(2, Abstract_tag);
      Field(ret, 0) = origin;
      Field(ret, 1) = size;
      CAMLreturn(ret);
    }

    |}
  ];
};
