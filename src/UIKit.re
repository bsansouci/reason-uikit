/*
 * Modeling Object Oriented using simple modules.
 * This only allows for 2 levels of subclassing, which is enough for now. We might bump that to 3.
 *
 * This allows us to leave the type open in "subclassable modules", like
 *
 *   type t('a) = objcT('a, uiview)
 *
 * and then the subclass will add its own refinement
 *
 *   type t2 = t(myuiview);
 *
 * This allows for all functions declared on `t('a)` to work on `t2`.
 */
type objcT('a, 'b) = {objc: 'a};

type nsobject;

external isNil : objcT('a, 'b) => bool = "isNil";

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

let _CGSizeMake = (width, height) => {width, height};

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

[@c.class]
module NSString = {
  type nsstring;
  /* Sealed type */
  type t = objcT(nsobject, nsstring);
  external newWithUTF8String : string => t = "NSString_newWithUTF8String";
  [%c.raw
    {|
    CAMLprim value NSString_newWithUTF8String(value s) {
      CAMLparam1(s);
      CAMLlocal1(ret);
      ret = caml_alloc_small(1, Abstract_tag);
      Field(ret, 0) = (value)[[NSString alloc] initWithUTF8String:String_val(s)];
      CAMLreturn(ret);
    }
  |}
  ];
  external _UTF8String : t => string = "NSString_UTF8String";
  [%c.raw
    {|
    CAMLprim value NSString_UTF8String(value nsstring) {
      CAMLparam1(nsstring);
      CAMLreturn(caml_copy_string(((NSString *)Field(nsstring, 0)).UTF8String));
    }
  |}
  ];
};

[@c.class]
module NSSet = {
  type t('a);

  external anyObject : t('a) => 'a = "NSSet_anyObject";
  [%c.raw {|
    CAMLprim value NSSet_anyObject(value set) {
      CAMLparam1(set);
      CAMLlocal1(ret);
      ret = caml_alloc_small(1, Abstract_tag);
      Field(ret, 0) = (value)[((NSSet *)Field(set, 0)) anyObject];
      CAMLreturn(ret);
    }
  |}];
};

[@c.class]
module UIColor = {
  type uicolor;
  type t = objcT(nsobject, uicolor);
  [@c.static] external redColor : unit => t = "";
  [@c.static] external greenColor : unit => t = "";
  [@c.static] external whiteColor : unit => t = "";
};

module rec UIViewController: {
  type uiviewcontroller;
  type t('a) = objcT('a, uiviewcontroller);
  let setView: (t('a), UIView.t('b)) => unit;
} = {
  type uiviewcontroller;
  type t('a) = objcT('a, uiviewcontroller);
  external setView : (t('a), UIView.t('b)) => unit =
    "UIViewController_setView";
}
[@c.class]
and UIEvent : {
  type t;
} = {
  type t;
}
[@c.class]
and UITouch : {
  type t;
  let locationInView : (t, UIView.t('a)) => _CGPoint;
} = {
  type t;

  external locationInView : (t, UIView.t('a)) => _CGPoint =
    "UITouch_locationInView";
  [%c.raw
    {|
    CAMLprim value UITouch_locationInView(value touch, value uiview) {
      CAMLparam2(touch, uiview);
      CAMLlocal1(ret);

      CGPoint locationInView = [((UITouch *) Field(touch, 0)) locationInView:((UIView *)Field(uiview, 0))];

      ret = caml_alloc_small(2, Double_array_tag);
      Double_field(ret, 0) = (double)locationInView.x;
      Double_field(ret, 1) = (double)locationInView.y;

      CAMLreturn(ret);
    }
  |}];
}
[@c.class]
and UIView: {
  type uiview;
  type t('a) = objcT('a, uiview);
  let _new: unit => t(nsobject);
  let newWithFrame: _CGRect => t(nsobject);
  let frame: t('a) => _CGRect;
  let setFrame: (t('a), _CGRect) => unit;
  let layoutSubviews: t('a) => unit;
  let sizeToFit: t('a) => unit;
  let sizeThatFits: (t('a), _CGSize) => _CGSize;
  let superview: t('a) => t('a);
  let setBackgroundColor: (t('a), UIColor.t) => unit;
  let addSubview: (t('a), t('b)) => unit;
  let setClipsToBounds: (t('a), bool) => unit;
  let addGestureRecognizer: (t('a), UIGestureRecognizer.t('b)) => unit;
} = {

  [%c.raw
    {|
  CAMLprim value UIView_newWithFrame(value frame) {
  CAMLparam1(frame);
  CAMLlocal1(ret);
  value origin = Field(frame, 0);
  value size = Field(frame, 1);
  CGRect f = CGRectMake(Double_field(origin, 0), Double_field(origin, 1), Double_field(size, 0), Double_field(size, 1));

  ret = caml_alloc_small(1, Abstract_tag);
  Field(ret, 0) = (value)[[UIView alloc] initWithFrame:f];
  CAMLreturn(ret);
}

CAMLprim value UIView_frame(value uiview) {
  CAMLparam1(uiview);
  CAMLlocal3(ret, origin, size);
  UIView *view = (UIView *)Field(uiview, 0);

  CGRect r = [view frame];

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

void UIView_setFrame(value uiview, value frame) {
  CAMLparam2(uiview, frame);
  UIView *view = (UIView *)Field(uiview, 0);
  value origin = Field(frame, 0);
  value size = Field(frame, 1);
  view.frame = CGRectMake(Double_field(origin, 0), Double_field(origin, 1), Double_field(size, 0), Double_field(size, 1));
  CAMLreturn0;
}

void UIView_addSubview(value view, value subview) {
  CAMLparam2(view, subview);
  [(UIView *)Field(view, 0) addSubview:(UIView *)Field(subview, 0)];
  CAMLreturn0;
}

void UIViewController_setView(value uiviewcontroller, value uiview) {
  CAMLparam2(uiviewcontroller, uiview);
  ((UIViewController *)Field(uiviewcontroller, 0)).view = (UIView *)Field(uiview, 0);
  CAMLreturn0;
}

CAMLprim value UIView_backgroundColor(value uiview) {
  CAMLparam1(uiview);
  CAMLreturn((value)[(UIView *)Field(uiview, 0) backgroundColor]);
}

void UIView_setBackgroundColor(value uiview, value uicolor) {
  CAMLparam2(uiview, uicolor);
  ((UIView *)Field(uiview, 0)).backgroundColor = (UIColor *)Field(uicolor, 0);
  CAMLreturn0;
}

void UIView_setClipsToBounds(value uiview, value b) {
  CAMLparam2(uiview, b);
  ((UIView *) Field(uiview, 0)).clipsToBounds = Int_val(b);
  CAMLreturn0;
}

void UIView_sizeToFit(value uiview) {
  [(UIView *)Field(uiview, 0) sizeToFit];
}

CAMLprim value UIView_sizeThatFits(value uiview, value size) {
  CAMLparam2(uiview, size);
  CAMLlocal1(ret);
  CGSize retSize = [(UIView *)Field(uiview, 0) sizeThatFits:CGSizeMake(Double_field(size, 0), Double_field(size, 1))];
  ret = caml_alloc_small(2, Double_array_tag);
  Double_field(ret, 0) = retSize.width;
  Double_field(ret, 1) = retSize.height;
  CAMLreturn(ret);
}

void mainReason(UIViewController *viewController) {
  CAMLparam0();
  CAMLlocal1(ret);

  ret = caml_alloc_small(1, Abstract_tag);
  Field(ret, 0) = (value)viewController;

  value *reason_main = caml_named_value("main");
  caml_callback(*reason_main, ret);
  CAMLreturn0;
}
|}
  ];
  type uiview;
  type t('a) = objcT('a, uiview);
  [@c.new] external _new : unit => t(nsobject) = "";
  /* [@c.new] external newWithFrame : _CGRect => t('a) = ""; */
  external newWithFrame : _CGRect => t(nsobject) = "UIView_newWithFrame";
  /* [@c.property] external frame : _CGRect = ""; */
  external frame : t('a) => _CGRect = "UIView_frame";
  external setFrame : (t('a), _CGRect) => unit = "UIView_setFrame";
  external layoutSubviews : t('a) => unit = "UIView_layoutSubviews";
  [%c.raw
    {|
    void UIView_layoutSubviews(value view) {
      CAMLparam1(view);
      [(UIView *)Field(view, 0) layoutSubviews];
      CAMLreturn0;
    }
  |}
  ];
  external sizeToFit : t('a) => unit = "UIView_sizeToFit";
  external sizeThatFits : (t('a), _CGSize) => _CGSize = "UIView_sizeThatFits";
  external superview : t('a) => t('a) = "UIView_superview";
  [%c.raw
    {|
    CAMLprim value UIView_superview(value v) {
      CAMLparam1(v);
      CAMLlocal1(ret);
      ret = caml_alloc_small(1, Abstract_tag);
      Field(ret, 0) = (long)[((UIView *)Field(v, 0)) superview];
      CAMLreturn(ret);
    }
  |}
  ];
  /*[@c.method]
    let layoutSubviews = (view) => {
      UIView.layoutSubviews(UIView.superview(view));
      print_endline("YO");
    };*/
  /*
     @Problem
     This is annoying, I had to subclass UIView to keep track of the closure written in Reason that
     registered to be called on layoutSubviews. I'd be nice to have a more generalized solution that
     avoids as much memory duplicated as possible.

     This function returns the callback to make sure the OCaml GC does not deallocate the closure, because we attach it to the uiview.
   */
  /*external setLayoutSubviews : (t('a), unit => unit) => unit =
      "UIView_setLayoutSubviews";
    external setTouchesBegan : (t('a), _CGPoint => unit) => unit =
      "UIView_setTouchesBegan";
    external setTouchesMoved : (t('a), _CGPoint => unit) => unit =
      "UIView_setTouchesMoved";
    external setTouchesEnded : (t('a), _CGPoint => unit) => unit =
      "UIView_setTouchesEnded";*/
  /* [@c.property] external backgroundColor : UIColor.t('a) = ""; */
  external backgroundColor : t('a) => UIColor.t = "UIView_backgroundColor";
  external setBackgroundColor : (t('a), UIColor.t) => unit =
    "UIView_setBackgroundColor";
  external addSubview : (t('a), t('b)) => unit = "UIView_addSubview";
  external setClipsToBounds : (t('a), bool) => unit =
    "UIView_setClipsToBounds";
  external addGestureRecognizer : (t('a), UIGestureRecognizer.t('b)) => unit =
    "UIView_addGestureRecognizer";
  [%c.raw
    {|
    void UIView_addGestureRecognizer(value view, value g) {
      CAMLparam2(view, g);
      [((UIView *)Field(view, 0)) addGestureRecognizer:(UIGestureRecognizer *)Field(g, 0)];
      CAMLreturn0;
    }
  |}
  ];
}
[@c.class]
and UIGestureRecognizer: {
  type gestureRecognizer;
  type t('a) = objcT('a, gestureRecognizer);
  type stateT =
    | UIGestureRecognizerStatePossible
    | UIGestureRecognizerStateBegan
    | UIGestureRecognizerStateChanged
    | UIGestureRecognizerStateEnded
    | UIGestureRecognizerStateCancelled
    | UIGestureRecognizerStateFailed
    | UIGestureRecognizerStateRecognized;
  let state: t('a) => stateT;
  let locationInView: (t('a), UIView.t('b)) => _CGPoint;
} = {
  type gestureRecognizer;
  type t('a) = objcT('a, gestureRecognizer);
  type stateT =
    | UIGestureRecognizerStatePossible
    | UIGestureRecognizerStateBegan
    | UIGestureRecognizerStateChanged
    | UIGestureRecognizerStateEnded
    | UIGestureRecognizerStateCancelled
    | UIGestureRecognizerStateFailed
    | UIGestureRecognizerStateRecognized;
  external state : t('a) => stateT = "UIGestureRecognizer_state";
  [%c.raw
    {|
    CAMLprim value UIGestureRecognizer_state(value g) {
      CAMLparam1(g);
      CAMLreturn(Val_int(((UIGestureRecognizer *) Field(g, 0)).state));
    }
  |}
  ];
  external locationInView : (t('a), UIView.t('b)) => _CGPoint =
    "UIGestureRecognizer_locationInView";
  [%c.raw
    {|
    CAMLprim value UIGestureRecognizer_locationInView(value gesture, value uiview) {
      CAMLparam2(gesture, uiview);
      CAMLlocal1(ret);

      CGPoint locationInView = [((UIGestureRecognizer *) Field(gesture, 0)) locationInView:((UIView *)Field(uiview, 0))];

      ret = caml_alloc_small(2, Double_array_tag);
      Double_field(ret, 0) = (double)locationInView.x;
      Double_field(ret, 1) = (double)locationInView.y;

      CAMLreturn(ret);
    }
  |}
  ];
}
[@c.class]
and UILongPressGestureRecognizer: {
  type uilongpressgesturerecognizer;
  type t2 = UIGestureRecognizer.t(uilongpressgesturerecognizer);
  /*let initWithTarget: (UIView.t, t => unit) => t;*/
} = {
  include UIGestureRecognizer;
  type uilongpressgesturerecognizer;
  type t2 = t(uilongpressgesturerecognizer);
  /*external initWithTarget : (UIView.t, t => unit) => t =
    "UILongPressGestureRecognizer_initWithTarget";*/
};

[@c.class]
module UILabel = {
  include UIView;
  type uilabel;
  type t2 = t(uilabel);
  [@c.new] external _new : unit => t2 = "";
  external text : t2 => NSString.t = "UILabel_text";
  [%c.raw
    {|
    CAMLprim value UILabel_text(value uilabel) {
      CAMLparam1(uilabel);
      CAMLlocal1(ret);
      ret = caml_alloc_small(1, Abstract_tag);
      Field(ret, 0) = (value)((UILabel *)Field(uilabel, 0)).text;
      CAMLreturn(ret);
    }
  |}
  ];
  external setText : (t2, NSString.t) => unit = "UILabel_setText";
  [%c.raw
    {|
    void UILabel_setText(value uilabel, value text) {
      CAMLparam2(uilabel, text);
      [((UILabel *)Field(uilabel, 0)) setText: (NSString *)Field(text, 0)];
      CAMLreturn0;
    }
  |}
  ];
};

[@c.class]
module UITextView = {
  include UIView;
  type uitextview;
  type t2 = t(uitextview);
  [@c.new] external _new : unit => t2 = "";
  /* heh subclassing works in our favor sometimes... */
  /*external newWithFrame : _CGRect => t = "UIView_newWithFrame";*/
  external setText : (t2, NSString.t) => unit = "UITextView_setText";
  [%c.raw
    {|
    void UITextView_setText(value uitextview, value text) {
      CAMLparam2(uitextview, text);
      [((UITextView *)Field(uitextview, 0)) setText:(NSString *)Field(text, 0)];
      CAMLreturn0;
    }
  |}
  ];
};

[@c.class]
module UIImage = {
  type uiimage;
  type t = objcT(nsobject, uiimage);
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
  type t2 = t(uiimageview);
  [@c.new] external _new : unit => t2 = "";
  [@c.new] external newWithImage : UIImage.t => t2 = "";
};

[@c.class]
module UIScrollView = {
  include UIView;
  type uiscrollview;
  type t2 = t(uiscrollview);
  [@c.new] external _new : unit => t2 = "";
  external newWithFrame : _CGRect => t2 = "UIScrollView_newWithFrame";
  [%c.raw
    {|
    CAMLprim value UIScrollView_newWithFrame(value frame) {
      CAMLparam1(frame);
      CAMLlocal1(ret);
      value origin = Field(frame, 0);
      value size = Field(frame, 1);
      CGRect f = CGRectMake(Double_field(origin, 0), Double_field(origin, 1), Double_field(size, 0), Double_field(size, 1));

      ret = caml_alloc_small(1, Abstract_tag);
      Field(ret, 0) = (value)[[UIScrollView alloc] initWithFrame:f];
      CAMLreturn(ret);
    }
  |}
  ];
  external setContentSize : (t2, _CGSize) => unit =
    "UIScrollView_setContentSize";
  [%c.raw
    {|
    void UIScrollView_setContentSize(value scrollview, value contentSize) {
      CAMLparam2(scrollview, contentSize);

      CGSize size = CGSizeMake(Double_field(contentSize, 0), Double_field(contentSize, 1));
      [((UIScrollView *)Field(scrollview, 0)) setContentSize:size];

      CAMLreturn0;
    }
  |}
  ];
};

[@c.class]
module UIScreen = {
  type uiscreen;
  type t = objcT(nsobject, uiscreen);
  [@c.static] external mainScreen : unit => t = "";
  external bounds : t => _CGRect = "UIScreen_bounds";
  [%c.raw
    {|
    CAMLprim value UIScreen_bounds(value screen) {
      CAMLparam1(screen);
      CAMLlocal3(ret, origin, size);

      CGRect r = [((UIScreen *)Field(screen, 0)) bounds];

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
