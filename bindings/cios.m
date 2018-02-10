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
}

- (void)layoutSubviews {
  if (_layoutSubviews != 0) {
    caml_callback(_layoutSubviews, Val_none);
  }
}

- (void)setLayoutSubviews:(value)layoutSubviews {
  _layoutSubviews = layoutSubviews;
}

@end

CAMLprim value UIView_new() {
  CAMLparam0();
  CAMLreturn((value)[MyUIView new]);
}

CAMLprim value UIView_newWithFrame(value frame) {
  CAMLparam1(frame);
  value origin = Field(frame, 0);
  value size = Field(frame, 1);
  CGRect f = CGRectMake(Double_field(origin, 0), Double_field(origin, 1), Double_field(size, 0), Double_field(size, 1));
  CAMLreturn((value)[[MyUIView alloc] initWithFrame:f]);
}

CAMLprim value UIView_frame(value uiview) {
  CAMLparam0();
  CAMLlocal3(ret, origin, size);
  MyUIView *view = (MyUIView *)uiview;

  CGRect r = [view frame];

  origin = caml_alloc_small(2, Double_array_tag);
  Double_field(origin, 0) = r.origin.x;
  Double_field(origin, 1) = r.origin.y;

  size = caml_alloc_small(2, Double_array_tag);
  Double_field(size, 0) = r.size.width;
  Double_field(size, 1) = r.size.height;

  ret = caml_alloc_small(2, Abstract_tag);
  Field(ret, 0) = origin;
  Field(ret, 1) = size;
  CAMLreturn(ret);
}

void UIView_setFrame(value uiview, value frame) {
  CAMLparam2(uiview, frame);
  MyUIView *view = (MyUIView *)uiview;
  value origin = Field(frame, 0);
  value size = Field(frame, 1);
  view.frame = CGRectMake(Double_field(origin, 0), Double_field(origin, 1), Double_field(size, 0), Double_field(size, 1));
  CAMLreturn0;
}

void UIView_layoutSubviews(value uiview) {
  CAMLparam1(uiview);
  MyUIView *view = (MyUIView *)uiview;
  [view layoutSubviews];
  CAMLreturn0;
}

CAMLprim value UIView_setLayoutSubviews(value uiview, value cb) {
  CAMLparam2(uiview, cb);
  CAMLlocal1(ret);
  MyUIView *view = (MyUIView *)uiview;
  ret = caml_alloc_small(1, Abstract_tag);
  Field(ret, 0) = cb;
  [view setLayoutSubviews:cb];
  CAMLreturn(ret);
}

void UIView_addSubview(value view, value subview) {
  CAMLparam2(view, subview);
  [(UIView *)view addSubview:(UIView *)subview];
  CAMLreturn0;
}

void UIViewController_setView(value uiviewcontroller, value uiview) {
  CAMLparam2(uiviewcontroller, uiview);
  ((UIViewController *)uiviewcontroller).view = (UIView *)uiview;
  CAMLreturn0;
}

void UIView_setBackgroundColor(value uiview, value uicolor) {
  CAMLparam2(uiview, uicolor);
  ((UIView *)uiview).backgroundColor = (UIColor *)uicolor;
  CAMLreturn0;
}

CAMLprim value UIColor_redColor() {
  CAMLparam0();
  CAMLreturn((value)[UIColor redColor]);
}

CAMLprim value UIColor_greenColor() {
  CAMLparam0();
  CAMLreturn((value)[UIColor greenColor]);
}

void UIView_setClipsToBounds(value uiview, value b) {
  CAMLparam2(uiview, b);
  ((UIView *) uiview).clipsToBounds = Int_val(b);
  CAMLreturn0;
}

void mainReason(UIViewController *viewController) {
  CAMLparam0();
  value *reasongl_main = caml_named_value("main");
  caml_callback(*reasongl_main, (value)viewController);
  CAMLreturn0;
}
