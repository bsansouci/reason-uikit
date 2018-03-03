#import "MainViewController.h"

extern void mainReason(MainViewController *);

@implementation MainViewController

- (void)loadView {
  mainReason(self);
}

@end

/*
 open MainViewController;

 MainViewController.setLoadView(self, () => {
  mainReason(self);
 });


 */
