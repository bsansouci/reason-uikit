open UIKit;

let sp = Printf.sprintf;

let main = mainController => {
  let v = UIView._new();
  let subview = UIView.newWithFrame(_CGRectMake(100., 100., 200., 200.));
  UIView.setBackgroundColor(subview, UIColor.redColor());
  UIView.setClipsToBounds(subview, true);
  let subview2 = UIView.newWithFrame(_CGRectMake(50., 50., 100., 100.));
  UIView.setBackgroundColor(subview2, UIColor.greenColor());
  let text = UILabel._new();
  UILabel.setText(text, NSString.newWithString("Drag me around!"));
  UIView.addSubview(subview, text);
  UIView.addSubview(subview, subview2);
  UIView.addSubview(v, subview);
  UIView.setLayoutSubviews(
    v,
    () => {
      let size = UIView.sizeThatFits(text, UIView.frame(subview).size);
      UIView.setFrame(text, { origin: { x: 0., y: 0. }, size });
    }
  );
  let oldX = ref(0.);
  let oldY = ref(0.);
  let dragging = ref(false);
  let frame = ref(UIView.frame(subview));
  UIView.setTouchesBegan(
    v,
    touchLocation => {
      frame := UIView.frame(subview);
      if (_CGRectContainsPoint(frame^, touchLocation)) {
        dragging := true;
        oldX := touchLocation.x;
        oldY := touchLocation.y;
      };
    }
  );
  UIView.setTouchesMoved(v, touchLocation =>
    if (dragging^) {
      let newFrame =
        _CGRectMake(
          frame^.origin.x +. touchLocation.x -. oldX^,
          frame^.origin.y +. touchLocation.y -. oldY^,
          frame^.size.width,
          frame^.size.height
        );
      UIView.setFrame(subview, newFrame);
    }
  );
  UIView.setTouchesEnded(v, touchLocation => dragging := false);
  UIViewController.setView(mainController, v);
  print_endline("Hello Sailor");
};

Callback.register("main", main);
