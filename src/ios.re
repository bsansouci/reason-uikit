open UIKit;

let sp = Printf.sprintf;

let main = mainController => {
  let bla = UIView._new();
  UIView.setFrame(bla, _CGRectMake(0., 0., 500., 500.));
  UIView.setBackgroundColor(bla, UIColor.whiteColor());
  let v = UIView._new();
  UIView.addSubview(v, bla);
  let image =
    switch (UIImage.imageNamed("shadow")) {
    | None => failwith("image not there dumb dumb")
    | Some(image) =>
      UIImage.resizableImageWithCapInsets(
        image,
        _UIEdgeInsetsMake(45., 45., 45., 45.)
      )
    };
  let cardShadow = UIImageView.newWithImage(image);
  UIView.setFrame(cardShadow, _CGRectMake(20., 20., 100., 100.));
  UIView.addSubview(v, cardShadow);
  UIViewController.setView(mainController, v);
};

Callback.register("main", main);
