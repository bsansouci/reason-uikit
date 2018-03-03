open UIKit;

let sp = Printf.sprintf;

let main = mainController => {
  let v = UIView._new();
  let subview = UIView.newWithFrame(_CGRectMake(100., 100., 100., 100.));
  UIView.setBackgroundColor(subview, UIColor.redColor());
  UIView.setClipsToBounds(subview, true);
  let subview2 = UIView.newWithFrame(_CGRectMake(50., 50., 100., 100.));
  UIView.setBackgroundColor(subview2, UIColor.greenColor());
  let text = UILabel._new();
  UILabel.setText(text, NSString.newWithString("Hello Sailor!"));
  UIView.addSubview(subview, text);
  UIView.addSubview(subview, subview2);
  UIView.addSubview(v, subview);
  UIView.setLayoutSubviews(
    v,
    () => {
      let size = UIView.sizeThatFits(text, UIView.frame(subview).size);
      UIView.setFrame(text, {
                              origin: {
                                x: 0.,
                                y: 0.
                              },
                              size
                            });
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
