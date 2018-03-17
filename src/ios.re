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
  let cardShadow = UIImageView.newWithImage(image);
  let frame =
    _CGRectMake(screenSize.size.width /. 2. -. 150., 100., 300., 120.);
  UIView.setFrame(cardShadow, frame);
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
  UITextView.setText(text, NSString.newWithUTF8String("Hello Sailor!"));
  UIView.addSubview(v, cardShadow);
  UIView.addSubview(v, text);
  UIViewController.setView(mainController, v);
};

Callback.register("main", main);
