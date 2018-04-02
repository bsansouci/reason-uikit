open UIKit;

let sp = Printf.sprintf;

let main = mainController => {
  let bla = UIView._new();
  let screenSize = UIScreen.(bounds(mainScreen()));
  UIView.setFrame(bla, screenSize);
  UIView.setBackgroundColor(bla, UIColor.whiteColor());
  let v = UIView._new();
  UIView.addSubview(v, bla);
  let edgeInsets = _UIEdgeInsetsMake(45., 45., 45., 45.);
  let image =
    switch (UIImage.imageNamed("shadow")) {
    | None => failwith("image not there dumb dumb")
    | Some(image) => UIImage.resizableImageWithCapInsets(image, edgeInsets)
    };
  let scrollview =
    UIScrollView.newWithFrame({
      origin: {
        x: 0.,
        y: 0.
      },
      size: screenSize.size
    });
  for (y in 0 to 6) {
    let cardShadow = UIImageView.newWithImage(image);
    let frame =
      _CGRectMake(
        screenSize.size.width /. 2. -. 150.,
        100. +. float_of_int(y) *. 120.,
        300.,
        120.
      );
    UIView.setFrame(cardShadow, frame);
    UIView.addSubview(scrollview, cardShadow);
    let text = UITextView._new();
    UITextView.setFrame(
      text,
      _CGRectMake(
        frame.origin.x +. edgeInsets.top,
        frame.origin.y +. edgeInsets.left,
        frame.size.width -. edgeInsets.left -. edgeInsets.right,
        frame.size.height -. edgeInsets.top -. edgeInsets.bottom
      )
    );
    UITextView.setText(
      text,
      NSString.newWithUTF8String("Hello Sailor " ++ string_of_int(y) ++ "!")
    );
    UIView.addSubview(scrollview, text);
    let padding = 0.;
    let handle =
      UIView.newWithFrame(
        _CGRectMake(
          screenSize.size.width /. 2. +. 150. +. padding,
          100. +. float_of_int(y) *. 120. +. 44.,
          32.,
          32.
        )
      );
    let g =
      UILongPressGestureRecognizer.initWithTarget(handle, g =>
        switch (UIGestureRecognizer.state(g)) {
        | UIGestureRecognizerStateChanged =>
          let pos = UIGestureRecognizer.locationInView(g, v);
          print_endline(
            "UIGestureRecognizerStateChanged x: "
            ++ string_of_float(pos.x)
            ++ " y: "
            ++ string_of_float(pos.y)
          );
        | _ => ()
        }
      );
    UIView.addGestureRecognizer(handle, g);
    UIView.setBackgroundColor(handle, UIColor.redColor());
    UIView.addSubview(scrollview, handle);
  };
  UIScrollView.setContentSize(
    scrollview,
    _CGSizeMake(screenSize.size.width, 7. *. 120. +. 100.)
  );
  UIView.addSubview(v, scrollview);
  UIViewController.setView(mainController, v);
};

Callback.register("main", main);
