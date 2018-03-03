#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <caml/callback.h>

#include <Foundation/Foundation.h>
#include <UIKit/UIkit.h>
#import <stdint.h>

#define Val_none Val_int(0)

static value Val_some(value v) {
  CAMLparam1(v);
  CAMLlocal1(some);
  some = caml_alloc_small(1, 0);
  Field(some, 0) = v;
  CAMLreturn(some);
}

CAMLprim value isNil(value objc) {
  CAMLparam1(objc);
  CAMLreturn(Int_val((id)(objc) == nil));
}

@interface MyUIView : UIView
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

@end


CAMLprim value UIView_new() {
  CAMLparam0();
  CAMLlocal1(ret);
  ret = caml_alloc_small(5, Abstract_tag);
  Field(ret, 0) = (value)[MyUIView new];
  Field(ret, 1) = Val_none;
  Field(ret, 2) = Val_none;
  Field(ret, 3) = Val_none;
  Field(ret, 4) = Val_none;
  CAMLreturn(ret);
}

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
  CAMLparam0();
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


// UIColor --------------------

CAMLprim value UIColor_redColor() {
  CAMLparam0();
  CAMLreturn((value)[UIColor redColor]);
}

CAMLprim value UIColor_greenColor() {
  CAMLparam0();
  CAMLreturn((value)[UIColor greenColor]);
}

// end UIColor

// UILabel ------------------------

CAMLprim value UILabel_new() {
  CAMLparam0();
  CAMLlocal1(ret);
  ret = caml_alloc_small(5, Abstract_tag);
  Field(ret, 0) = (value)[UILabel new];
  Field(ret, 1) = Val_none;
  Field(ret, 2) = Val_none;
  Field(ret, 3) = Val_none;
  Field(ret, 4) = Val_none;
  CAMLreturn(ret);
}

void UILabel_setText(value uilabel, value text) {
  CAMLparam2(uilabel, text);
  ((UILabel *)Field(uilabel, 0)).text = (NSString *) text;
  CAMLreturn0;
}

CAMLprim value UILabel_text(value uilabel) {
  CAMLparam1(uilabel);
  CAMLreturn((value)((UILabel *)Field(uilabel, 0)).text);
}

// end UILabel



// NSString ------------------------

CAMLprim value NSString_newWithString(value str) {
  CAMLparam1(str);
  CAMLreturn((value)[[NSString alloc] initWithUTF8String:String_val(str)]);
}

CAMLprim value NSString_UTF8String(value nsstring) {
  CAMLparam1(nsstring);
  CAMLreturn(caml_copy_string(((NSString *)nsstring).UTF8String));
}

// end NSString

void mainReason(UIViewController *viewController) {
  CAMLparam0();
  value *reason_main = caml_named_value("main");
  caml_callback(*reason_main, (value)viewController);
  CAMLreturn0;
}
