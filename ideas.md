

Adding type annotations solve the issue I was having. It's like inference doesn't work, but at least my crappy ppx works!

---------- 

I think this isn't amazing, because you need all those weird annotations and there is no "module" equivalent in Obcj/Swift. So we kinda want classes and objects... I've been avoiding those because bleh.
Then again, you shouldn't need actual subclassing often.

Ok i fixed the previous problem #2 using `type objcT('a, 'b);` and then making the left side the subclass and the right the superclass. 
So you get
```reason
type objcT('a, 'b) => {objc: 'b};

[@c.class]
module UIView = {
  type uiview;
  type t('a) = objcT('a, uiview);
  [@c.new] external _new : unit => t(nsobject) = "";
};

[@c.class]
module MyView = {
  include UIView;
  
  type myview;
  type t2 = t(myview);
  
  /* This returns a `t2`, so `UIView` functions will work on it but 
   * specialized functions won't work on `t`s other than `t2`s. 
   */
  [@c.new] external _new : unit => t2 = "";
  
  [@c.method]
  let layoutSubviews = view => {
    UIView.layoutSubviews(UIView.superview(view));
    print_endline("YO");
  };
  
  let specialized_method_on_only_t2 = (v: t2) => print_endline("hey t2");
};
```

------------ 

This works, but it has two flaws:
1) you have to re-write the externals that instantiate the subclass inside the subclass
2) you can't have specialized functions in there that work on `t2` only. I return `t` here so that the user can call `UIView.frame(v)` where `let v = MyView._new()`. 
```reason
[@c.class "UIView"]
module MyView = {
  include UIView;
  type myview;
  type t2 = objcT(myview);

  [@c.new] external _new : unit => t = "";
  [@c.method]
  let layoutSubviews = (view) => {
    UIView.layoutSubviews(UIView.superview(view));
    print_endline("YO");
  };
};
``` 

-------------

I started with the idea of using modules as a simple way to bind to objc classes. It worked well until I hit the issue of overriding methods.
I made a `c.method` which would generate a method inside a class given the module name and method name. The is great, but then should you do `MyUIView._new()` or `UIView._new()`? Well probably the first one, but how do instantiate it without re-writing the external?

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
