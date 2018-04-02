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
  type t = objcT(nsstring);
  external newWithUTF8String : string => t = "NSString_newWithUTF8String";
  [%c.raw
    {|
    CAMLprim value NSString_newWithUTF8String(value s) {
      CAMLparam1(s);
      CAMLreturn((value)[[NSString alloc] initWithUTF8String:String_val(s)]);
    }
  |}
  ];
  external _UTF8String : t => string = "NSString_UTF8String";
  [%c.raw
    {|
    CAMLprim value NSString_UTF8String(value nsstring) {
      CAMLparam1(nsstring);
      CAMLreturn(caml_copy_string(((NSString *)nsstring).UTF8String));
    }
  |}
  ];
};

[@c.class]
module UIColor = {
  type uicolor;
  type t = objcT(uicolor);
  [@c.static] external redColor : unit => t = "";
  [@c.static] external greenColor : unit => t = "";
  [@c.static] external whiteColor : unit => t = "";
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
  let addGestureRecognizer: (t, UIGestureRecognizer.t) => unit;
} = {
  [%c.raw {|
@interface MyUIView : UIView

@property(nonatomic, retain) UIGestureRecognizer *gesture1;
@property(nonatomic, assign) value gesture1Callback;

@property(nonatomic, retain) UIGestureRecognizer *gesture2;
@property(nonatomic, assign) value gesture2Callback;

@end

@implementation MyUIView {
  value _layoutSubviews;
  value _touchesBegan;
  value _touchesMoved;
  value _touchesEnded;
}

- (void)layoutSubviews {
  if (_layoutSubviews != 0) {
    caml_callback(_layoutSubviews, Val_none);
  }
}

- (void)setLayoutSubviews:(value)layoutSubviews {
  _layoutSubviews = layoutSubviews;
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
  [super touchesBegan:touches withEvent:event];
  if (_touchesBegan != 0) {
    UITouch* touchEvent = [touches anyObject];
    CGPoint locationInView = [touchEvent locationInView:self];
    CAMLparam0();
    CAMLlocal1(ret);
    ret = caml_alloc_small(2, Double_array_tag);
    Double_field(ret, 0) = (double)locationInView.x;
    Double_field(ret, 1) = (double)locationInView.y;
    caml_callback(_touchesBegan, ret);
    CAMLreturn0;
  }
}

- (void)setTouchesBegan:(value)touchesBegan {
  _touchesBegan = touchesBegan;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
  [super touchesMoved:touches withEvent:event];
  if (_touchesMoved != 0) {
    UITouch* touchEvent = [touches anyObject];
    CGPoint locationInView = [touchEvent locationInView:self];
    CAMLparam0();
    CAMLlocal1(ret);
    ret = caml_alloc_small(2, Double_array_tag);
    Double_field(ret, 0) = (double)locationInView.x;
    Double_field(ret, 1) = (double)locationInView.y;
    caml_callback(_touchesMoved, ret);
    CAMLreturn0;
  }
}

- (void)setTouchesMoved:(value)touchesMoved {
  _touchesMoved = touchesMoved;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
  [super touchesEnded:touches withEvent:event];
  if (_touchesEnded != 0) {
    UITouch* touchEvent = [touches anyObject];
    CGPoint locationInView = [touchEvent locationInView:self];
    CAMLparam0();
    CAMLlocal1(ret);
    ret = caml_alloc_small(2, Double_array_tag);
    Double_field(ret, 0) = (double)locationInView.x;
    Double_field(ret, 1) = (double)locationInView.y;
    caml_callback(_touchesEnded, ret);
    CAMLreturn0;
  }
}

- (void)setTouchesEnded:(value)touchesEnded {
  _touchesEnded = touchesEnded;
}

- (void)handleGesture:(UIGestureRecognizer *)recognizer {
  CAMLparam0();
  if (recognizer == _gesture1) {
    CAMLlocal1(ret);
    ret = caml_alloc_small(1, Abstract_tag);
    Field(ret, 0) = (long)_gesture1;
    caml_callback(_gesture1Callback, ret);
  } else if (recognizer == _gesture2) {
    CAMLlocal1(ret);
    ret = caml_alloc_small(1, Abstract_tag);
    Field(ret, 0) = (long)_gesture2;
    caml_callback(_gesture2Callback, ret);
  }
  CAMLreturn0;
}

@end


CAMLprim value UIView_newWithFrame(value frame) {
  CAMLparam1(frame);
  CAMLlocal1(ret);
  value origin = Field(frame, 0);
  value size = Field(frame, 1);
  CGRect f = CGRectMake(Double_field(origin, 0), Double_field(origin, 1), Double_field(size, 0), Double_field(size, 1));

  ret = caml_alloc_small(5, Abstract_tag);
  Field(ret, 0) = (value)[[MyUIView alloc] initWithFrame:f];
  Field(ret, 1) = Val_none;
  Field(ret, 2) = Val_none;
  Field(ret, 3) = Val_none;
  Field(ret, 4) = Val_none;
  CAMLreturn(ret);
}

CAMLprim value UIView_frame(value uiview) {
  CAMLparam1(uiview);
  CAMLlocal3(ret, origin, size);
  MyUIView *view = (MyUIView *)Field(uiview, 0);

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
  MyUIView *view = (MyUIView *)Field(uiview, 0);
  value origin = Field(frame, 0);
  value size = Field(frame, 1);
  view.frame = CGRectMake(Double_field(origin, 0), Double_field(origin, 1), Double_field(size, 0), Double_field(size, 1));
  CAMLreturn0;
}

void UIView_layoutSubviews(value uiview) {
  CAMLparam1(uiview);
  MyUIView *view = (MyUIView *)Field(uiview, 0);
  [view layoutSubviews];
  CAMLreturn0;
}

void UIView_setLayoutSubviews(value uiview, value cb) {
  CAMLparam2(uiview, cb);
  MyUIView *view = (MyUIView *)Field(uiview, 0);
  Field(uiview, 1) = Val_some(cb);
  [view setLayoutSubviews:cb];
  CAMLreturn0;
}

void UIView_setTouchesBegan(value uiview, value cb) {
  CAMLparam2(uiview, cb);
  MyUIView *view = (MyUIView *)Field(uiview, 0);
  Field(uiview, 2) = Val_some(cb);
  [view setTouchesBegan:cb];
  CAMLreturn0;
}

void UIView_setTouchesMoved(value uiview, value cb) {
  CAMLparam2(uiview, cb);
  MyUIView *view = (MyUIView *)Field(uiview, 0);
  Field(uiview, 3) = Val_some(cb);
  [view setTouchesMoved:cb];
  CAMLreturn0;
}

void UIView_setTouchesEnded(value uiview, value cb) {
  CAMLparam2(uiview, cb);
  MyUIView *view = (MyUIView *)Field(uiview, 0);
  Field(uiview, 4) = Val_some(cb);
  [view setTouchesEnded:cb];
  CAMLreturn0;
}

void UIView_addSubview(value view, value subview) {
  CAMLparam2(view, subview);
  [(UIView *)Field(view, 0) addSubview:(UIView *)Field(subview, 0)];
  CAMLreturn0;
}

void UIViewController_setView(value uiviewcontroller, value uiview) {
  CAMLparam2(uiviewcontroller, uiview);
  ((UIViewController *)uiviewcontroller).view = (UIView *)Field(uiview, 0);
  CAMLreturn0;
}

CAMLprim value UIView_backgroundColor(value uiview) {
  CAMLparam1(uiview);
  CAMLreturn((value)[(UIView *)Field(uiview, 0) backgroundColor]);
}

void UIView_setBackgroundColor(value uiview, value uicolor) {
  CAMLparam2(uiview, uicolor);
  ((UIView *)Field(uiview, 0)).backgroundColor = (UIColor *)uicolor;
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

// end UIView

// end NSString

void mainReason(UIViewController *viewController) {
  CAMLparam0();
  value *reason_main = caml_named_value("main");
  caml_callback(*reason_main, (value)viewController);
  CAMLreturn0;
}

CAMLprim value UILongPressGestureRecognizer_initWithTarget(value target, value cb) {
  CAMLparam2(target, cb);
  CAMLlocal1(ret);

  MyUIView *view = (MyUIView *)Field(target, 0);
  UILongPressGestureRecognizer *g =
    [[UILongPressGestureRecognizer alloc] initWithTarget:view
                                                  action:@selector(handleGesture:)];
  if (!view.gesture1) {
    view.gesture1 = g;
    view.gesture1Callback = cb;
  } else {
    view.gesture2 = g;
    view.gesture2Callback = cb;
  }

  ret = caml_alloc_small(1, Abstract_tag);
  Field(ret, 0) = (long)g;

  CAMLreturn(ret);
}

  |}];

  type uiview;
  type t = {
    objc: objcT(uiview),
    mutable layoutSubviews: option(unit => unit),
    mutable touchesBegan: option(_CGPoint => unit),
    mutable touchesMoved: option(_CGPoint => unit),
    mutable touchesEnded: option(_CGPoint => unit)
  };
  [@c.new] external _new : unit => t = "";
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
  external addGestureRecognizer : (t, UIGestureRecognizer.t) => unit =
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
  type t;
  type stateT =
    | UIGestureRecognizerStatePossible
    | UIGestureRecognizerStateBegan
    | UIGestureRecognizerStateChanged
    | UIGestureRecognizerStateEnded
    | UIGestureRecognizerStateCancelled
    | UIGestureRecognizerStateFailed
    | UIGestureRecognizerStateRecognized;
  let state: t => stateT;
  let locationInView: t => UIView.t => _CGPoint;
} = {
  type gestureRecognizer;
  type t = objcT(gestureRecognizer);
  type stateT =
    | UIGestureRecognizerStatePossible
    | UIGestureRecognizerStateBegan
    | UIGestureRecognizerStateChanged
    | UIGestureRecognizerStateEnded
    | UIGestureRecognizerStateCancelled
    | UIGestureRecognizerStateFailed
    | UIGestureRecognizerStateRecognized;
  external state : t => stateT = "UIGestureRecognizer_state";
  [%c.raw
    {|
    CAMLprim value UIGestureRecognizer_state(value g) {
      CAMLparam1(g);
      CAMLreturn(Val_int(((UIGestureRecognizer *) Field(g, 0)).state));
    }
  |}
  ];

  external locationInView : t => UIView.t => _CGPoint = "UIGestureRecognizer_locationInView";
  [%c.raw {|
    CAMLprim value UIGestureRecognizer_locationInView(value gesture, value uiview) {
      CAMLparam2(gesture, uiview);
      CAMLlocal1(ret);

      CGPoint locationInView = [((UIGestureRecognizer *) Field(gesture, 0)) locationInView:((UIView *)Field(uiview, 0))];

      ret = caml_alloc_small(2, Double_array_tag);
      Double_field(ret, 0) = (double)locationInView.x;
      Double_field(ret, 1) = (double)locationInView.y;

      CAMLreturn(ret);
    }
  |}];
}
[@c.class]
and UILongPressGestureRecognizer: {
  type t = UIGestureRecognizer.t;
  let initWithTarget: (UIView.t, t => unit) => t;
} = {
  include UIGestureRecognizer;
  external initWithTarget : (UIView.t, t => unit) => t =
    "UILongPressGestureRecognizer_initWithTarget";
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
    CAMLprim value UILabel_text(value uilabel) {
      CAMLparam1(uilabel);
      CAMLreturn((value)((UILabel *)Field(uilabel, 0)).text);
    }
  |}
  ];
  external setText : (t, NSString.t) => unit = "UILabel_setText";
  [%c.raw
    {|
    void UILabel_setText(value uilabel, value text) {
      CAMLparam2(uilabel, text);
      [((UILabel *)Field(uilabel, 0)) setText: (NSString *)text];
      CAMLreturn0;
    }
  |}
  ];
};

[@c.class]
module UITextView = {
  include UIView;
  type uitextview;
  type t2 = objcT(uitextview);
  [@c.new] external _new : unit => t = "";
  /* heh subclassing works in our favor sometimes... */
  /*external newWithFrame : _CGRect => t = "UIView_newWithFrame";*/
  external setText : (t, NSString.t) => unit = "UITextView_setText";
  [%c.raw
    {|
    void UITextView_setText(value uitextview, value text) {
      CAMLparam2(uitextview, text);
      [((UITextView *)Field(uitextview, 0)) setText:(NSString *)text];
      CAMLreturn0;
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

[@c.class]
module UIScrollView = {
  include UIView;
  type uiscrollview;
  type t3 = objcT(uiscrollview);
  [@c.new] external _new : unit => t = "";
  external newWithFrame : _CGRect => t = "UIScrollView_newWithFrame";
  [%c.raw
    {|
    CAMLprim value UIScrollView_newWithFrame(value frame) {
      CAMLparam1(frame);
      CAMLlocal1(ret);
      value origin = Field(frame, 0);
      value size = Field(frame, 1);
      CGRect f = CGRectMake(Double_field(origin, 0), Double_field(origin, 1), Double_field(size, 0), Double_field(size, 1));

      ret = caml_alloc_small(5, Abstract_tag);
      Field(ret, 0) = (value)[[UIScrollView alloc] initWithFrame:f];
      Field(ret, 1) = Val_none;
      Field(ret, 2) = Val_none;
      Field(ret, 3) = Val_none;
      Field(ret, 4) = Val_none;
      CAMLreturn(ret);
    }
  |}
  ];
  external setContentSize : (t, _CGSize) => unit =
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
  type t = uiscreen;
  [@c.static] external mainScreen : unit => t = "";
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
