#include "ezOptionParser.hpp"
#define MAHI_GUI_NO_CONSOLE
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;
using namespace  ez;

class ArgumentParser : public OptionParser, public Application
{
public:
  ArgumentParser() : OptionParser()
  {
    std::string fontFile = "fonts/custom.ttf";
    std::string icoFile = "fonts/ico.ttf";
    if (CheckFileExistence(fontFile)) {
      ImFont *pFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(fontFile.c_str(), 18.0f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
      if (CheckFileExistence(icoFile)) {
        ImFontConfig config;
        config.MergeMode = true;
        config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
        static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        ImGui::GetIO().Fonts->AddFontFromFileTTF(icoFile.c_str(), 13.0f, &config, icon_ranges);
      }
      ImGui::GetIO().Fonts->Build();
      ImGui::GetIO().FontDefault = pFont;
    }
  };

  // Override update (called once per frame)
  void update() override
  {
    float x, y;
    x = 150;
    y = this->groups.size() * 25.0;
    ImGui::SetNextWindowSize(ImVec2(x, y), ImGuiCond_FirstUseEver);
    // App logic and/or ImGui code goes here
    ImGui::Begin(overview.c_str(), &open, ImGuiWindowFlags_NoCollapse);
    //ImGui::StyleColorsLight();
    const float windowScale = 1.2f;
    ImGui::SetWindowFontScale(windowScale);
    int i = 1;
    for (auto it = this->groups.begin(); it != this->groups.end(); ++it, ++i) {
      ImGui::Separator();
      OptionGroup *og = *it;
      if(og->flags[0] == "-h")
        continue;
      std::string label = "##" + std::to_string(i);
      ImGuiDataType imtype;
      switch (og->optType) {
      case EZ_INT8: {
        char example = 0;
        imtype = ImGuiDataType_S8;
        og->getMinMax(example);
      }
      break;
      case EZ_UINT8: {
        unsigned char example = 0;
        imtype = ImGuiDataType_U8;
        og->getMinMax(example);
      }
      break;
      case EZ_INT16: {
        short example = 0;
        imtype = ImGuiDataType_S16;
        og->getMinMax(example);
      }
      break;
      case EZ_UINT16: {
        unsigned short example = 0;
        imtype = ImGuiDataType_U16;
        og->getMinMax(example);
      }
      break;
      case EZ_INT32: {
        int example = 0;
        imtype = ImGuiDataType_S32;
        og->getMinMax(example);
      }
      break;
      case EZ_UINT32: {
        unsigned int example = 0;
        imtype = ImGuiDataType_U32;
        og->getMinMax(example);
      }
      break;
      case EZ_INT64: {
        long example = 0;
        imtype = ImGuiDataType_S64;
        og->getMinMax(example);
      }
      break;
      case EZ_UINT64: {
        unsigned long example = 0;
        imtype = ImGuiDataType_U64;
        og->getMinMax(example);
      }
      break;
      case EZ_FLOAT: {
        float example = 0;
        imtype = ImGuiDataType_Float;
        og->getMinMax(example);
      }
      break;
      case EZ_DOUBLE: {
        double example = 0;
        imtype = ImGuiDataType_Double;
        og->getMinMax(example);
      }
      break;
      default:
        ;
      }
      switch (og->optType) {
      case EZ_NOTYPE:
      case EZ_BOOL: {
        //add checkbox
        ImGui::Checkbox(og->help.c_str(), &(og->isSet));
      }
      break;
      case EZ_INT8:
      case EZ_UINT8:
      case EZ_INT16:
      case EZ_UINT16:
      case EZ_INT32:
      case EZ_UINT32:
      case EZ_INT64:
      case EZ_UINT64:
      case EZ_FLOAT:
      case EZ_DOUBLE: {
        if (og->validValues.size() > 0) {
          //add combo
          std::vector<std::string> &vecValid = og->validValues;
          int nIDX = 0;
          ImGui::Text(og->help.c_str());
          ImGuiComboFlags flags = 0;
          size_t &item_current_idx = og->validIndex;
          if (ImGui::BeginCombo(label.c_str(), vecValid[item_current_idx].c_str(), flags)) {
            for (size_t n = 0; n < vecValid.size(); n++) {
              const bool is_selected = (item_current_idx == n);
              if (ImGui::Selectable(vecValid[n].c_str(), is_selected))
                item_current_idx = n;
              if (is_selected) {
                ImGui::SetItemDefaultFocus();
                og->args.resize(1);
                og->args[0].resize(1);
                og->args[0][0] = vecValid[n];
                og->isSet = true;
              }
            }
            ImGui::EndCombo();
          }
        } else {
          //add int drag
          ImGui::Text(og->help.c_str());
          if (ImGui::DragScalarN(label.c_str(), imtype, og->scalaValues, og->expectArgs, 1.0f, &og->minValid, &og->maxValid)) {
            size_t scalaSize = og->expectArgs;
            og->isSet = true;
            og->args.resize(1);
            og->args[0].resize(scalaSize);
            for (size_t i = 0; i < scalaSize; ++i) {
              og->getScalaValue(i, og->args[0][i]);
            }
          }
        }
      }
      break;
      case EZ_TEXT: {
        if (og->validValues.size() > 0) {
          //add combo
          std::vector<std::string> &vecValid = og->validValues;
          int nIDX = 0;
          ImGui::Text(og->help.c_str());
          ImGuiComboFlags flags = 0;
          size_t &item_current_idx = og->validIndex;
          if (ImGui::BeginCombo(label.c_str(), vecValid[item_current_idx].c_str(), flags)) {
            for (size_t n = 0; n < vecValid.size(); n++) {
              const bool is_selected = (item_current_idx == n);
              if (ImGui::Selectable(vecValid[n].c_str(), is_selected))
                item_current_idx = n;
              if (is_selected) {
                ImGui::SetItemDefaultFocus();
                og->args.resize(1);
                og->args[0].resize(1);
                og->args[0][0] = vecValid[n];
                og->isSet = true;
              }
            }
            ImGui::EndCombo();
          }
        } else {
          //input
          ImGui::Text(og->help.c_str());
          if (ImGui::InputText(label.c_str(), &og->defaults)) {
            og->args.resize(1);
            og->args[0].resize(1);
            og->args[0][0] = og->defaults;
            og->isSet = true;
          }
        }
      }
      break;
      case EZ_FILE: {
        //file
        ImGui::Text(og->help.c_str());
        ImGui::InputText(label.c_str(), &og->defaults, ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        std::string iconLabel = ICON_FA_FILE + label;
        if (ImGui::Button(iconLabel.c_str() )) {
          if(open_dialog(og->defaults) == DialogOkay) {
            og->args.resize(1);
            og->args[0].resize(1);
            og->args[0][0] = og->defaults;
            og->isSet = true;
          }
        }
      }
      break;
      case EZ_DIR: {
        //dir
        ImGui::Text(og->help.c_str());
        ImGui::InputText(label.c_str(), &og->defaults, ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        std::string iconLabel = ICON_FA_FOLDER_OPEN + label;
        if (ImGui::Button(iconLabel.c_str())) {
          if (pick_dialog(og->defaults) == DialogOkay) {
            og->args.resize(1);
            og->args[0].resize(1);
            og->args[0][0] = og->defaults;
            og->isSet = true;
          }
        }
      }
      break;
      case EZ_OUTPUT: {
        //save
        ImGui::Text(og->help.c_str());
        ImGui::InputText(label.c_str(), &og->defaults, ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        std::string iconLabel = ICON_FA_SAVE + label;
        if (ImGui::Button(iconLabel.c_str())) {
          if (save_dialog(og->defaults) == DialogOkay) {
            og->args.resize(1);
            og->args[0].resize(1);
            og->args[0][0] = og->defaults;
            og->isSet = true;
          }
        }
      }
      break;
      default:
        break;
      }
    }
    static bool valid = true;
    ImGui::Separator();
    ImGui::InvisibleButton("##invisible", ImVec2(10, 30));
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 120);
    if (ImGui::Button("OK", ImVec2(50, 30))) {
      this->errors = "";
      if (!checkValid(this->errors)) {
        valid = false;
      } else {
        valid = true;
        ok = true;
        quit();
      }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(60, 30))) {
      quit();
    }
    if(!valid) {
      ImGui::InputTextMultiline("##errorInfo", &this->errors);
    }
    ImGui::End();
    if (!open)
      quit();
  }

  inline virtual bool parse(int argc, const char *const *argv)
  {
    if (argc > 1) {
      return OptionParser::parse(argc, argv);
    }
    return ok;
  }
protected:
  bool ok = false;
  bool open = true;
};