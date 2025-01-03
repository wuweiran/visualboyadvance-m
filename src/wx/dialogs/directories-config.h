#ifndef VBAM_WX_DIALOGS_DIRECTORIES_CONFIG_H_
#define VBAM_WX_DIALOGS_DIRECTORIES_CONFIG_H_

#include "wx/dialogs/base-dialog.h"

namespace dialogs {

// Manages the directories configuration dialog.
class DirectoriesConfig : public BaseDialog {
public:
    static DirectoriesConfig* NewInstance(wxWindow* parent);
    ~DirectoriesConfig() override = default;

private:
    // The constructor is private so initialization has to be done via the
    // static method. This is because this class is destroyed when its
    // owner, `parent` is destroyed. This prevents accidental deletion.
    DirectoriesConfig(wxWindow* parent);
};

}  // namespace dialogs

#endif  // VBAM_WX_DIALOGS_DIRECTORIES_CONFIG_H_
