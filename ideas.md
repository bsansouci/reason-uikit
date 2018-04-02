UIView.addMethod(handle, "longPressRecognized", () => {
});




```reason
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

 ```
