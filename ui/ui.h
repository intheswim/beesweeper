#pragma once

#include "UILabel.h"
#include "UIButton.h"
#include "UIBevel.h"
#include "UICheckbox.h"
#include "UISlider.h"
#include "UIManager.h"

#include <memory>

typedef std::shared_ptr<UIButton> UIButtonPtr;
typedef std::shared_ptr<UILabel> UILabelPtr;
typedef std::shared_ptr<UISlider> UISliderPtr;
typedef std::shared_ptr<UICheckbox> UICheckboxPtr;
typedef std::shared_ptr<UIBevel> UIBevelPtr;
typedef std::unique_ptr<UIManager> UIManagerPtr;

