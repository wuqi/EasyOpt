#include "ezOptionParser.hpp"
#if defined(_WIN32)
#include "windows.h"
#endif
#include <Mahi/Gui.hpp>
#include <Mahi/Util.hpp>

using namespace mahi::gui;
using namespace mahi::util;
using namespace  ez;

class ArgumentParser : public OptionParser, public Application
{
public:
  ArgumentParser(int c, const char **v) : OptionParser()
  {
    argc = c;
    argv = v;
    if (argc == 1) {
#if defined(_WIN32)
      ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif
    }
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
      ImGui::SetNextItemWidth(-1.0f);
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
          ImGui::SetNextItemWidth(-1.0f);
          ImGuiComboFlags flags = 0;
          size_t &item_current_idx = og->validIndex;
          if (ImGui::BeginCombo(label.c_str(), vecValid[item_current_idx].c_str(), flags)) {
            for (size_t n = 0; n < vecValid.size(); n++) {
              const bool is_selected = (item_current_idx == n);
              if (ImGui::Selectable(vecValid[n].c_str(), is_selected)) {
                item_current_idx = n;
                og->args.resize(1);
                og->args[0].resize(1);
                og->args[0][0] = vecValid[n];
                og->isSet = true;
              }
              if (is_selected) {
                ImGui::SetItemDefaultFocus();
              }
            }
            ImGui::EndCombo();
          }
        } else {
          //add int drag
          ImGui::Text(og->help.c_str());
          ImGui::SetNextItemWidth(-1.0f);
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
          ImGui::SetNextItemWidth(-1.0f);
          ImGuiComboFlags flags = 0;
          size_t &item_current_idx = og->validIndex;
          if (ImGui::BeginCombo(label.c_str(), vecValid[item_current_idx].c_str(), flags)) {
            for (size_t n = 0; n < vecValid.size(); n++) {
              const bool is_selected = (item_current_idx == n);
              if (ImGui::Selectable(vecValid[n].c_str(), is_selected)) {
                item_current_idx = n;
                og->args.resize(1);
                og->args[0].resize(1);
                og->args[0][0] = vecValid[n];
                og->isSet = true;
              }
              if (is_selected) {
                ImGui::SetItemDefaultFocus();
              }
            }
            ImGui::EndCombo();
          }
        } else {
          //input
          ImGui::Text(og->help.c_str());
          ImGui::SetNextItemWidth(-1.0f);
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
    float xpos = ImGui::GetWindowContentRegionMax().x / 3.0;
    ImGui::Separator();
    ImGui::SameLine(xpos);
    if (ImGui::Button("OK", ImVec2(xpos - 2, 30))) {
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
    if (ImGui::Button("Cancel", ImVec2(xpos - 2, 30))) {
      quit();
    }
    if(!valid) {
      ImGui::SetNextItemWidth(-1.0f);
      ImGui::InputTextMultiline("##errorInfo", &this->errors);
    }
    ImGui::End();
    if (!open)
      quit();
  }

  inline virtual bool parse()
  {
    if (argc > 1) {
      return OptionParser::parse(argc, argv);
    }
    return ok;
  };


  /**
  * @brief 检查输入参数格式是否正确
  * @param out 输出提示信息
  * @return 输入参数格式是否正确
  */
  inline virtual bool checkValid( std::string &out )
  {
    int i, j;
    bool isValid = true;
    /*check required*/
    for( i = 0; i < ( int )groups.size(); ++i ) {
      if( groups[i]->isRequired && !groups[i]->isSet ) {
        if(argc > 1)
          out.append("ERROR: Argument ")\
          .append(groups[i]->flags[0]).append(" must be set.\n");
        else
          out.append("ERROR: Argument ")\
          .append(groups[i]->help).append(" must be set.\n");
        isValid = false;
      }
    }
    /*check expectArgs*/
    for( i = 0; i < ( int )groups.size(); ++i ) {
      OptionGroup *g = groups[i];
      if( g->isSet ) {
        if( g->expectArgs != 0 && g->args.empty() ) {
          if(argc > 1)
            out.append( "ERROR: Got unexpected number of arguments for option " ) \
            .append( g->flags[0] ).append( ".\n" );
          else
            out.append("ERROR: Got unexpected number of arguments for option ") \
            .append(g->help).append(".\n");
          isValid = false;
          continue;
        }
        for( j = 0; j < ( int )g->args.size(); ++j ) {
          if( ( g->expectArgs != -1 && g->expectArgs != ( int )g->args[j].size() )
              || ( g->expectArgs == -1 && g->args[j].size() == 0 ) ) {
            if(argc > 1)
              out.append( "ERROR: Got unexpected number of arguments for option " ) \
              .append( g->flags[0] ).append( ".\n" );
            else
              out.append("ERROR: Got unexpected number of arguments for option ") \
              .append(g->help).append(".\n");
            isValid = false;
          }
        }
      }
    }
    /*check arguments validation*/
    std::vector<std::string> tempOptions;
    for( i = 0; i < ( int )groups.size(); ++i ) {
      if( !groups[i]->validate( tempOptions ) ) {
        for( j = 0; j < ( int )tempOptions.size(); ++j ) {
          if(argc > 1)
            out.append( "ERROR: Got invalid argument \"" ) \
            .append( tempOptions[j] ).append( "\" for option " ) \
            .append( groups[i]->flags[0] ).append( ".\n" );
          else
            out.append("ERROR: Got invalid argument \"") \
            .append(tempOptions[j]).append("\" for option ") \
            .append(groups[i]->help).append(".\n");
          isValid = false;
        }
      }
      tempOptions.clear();
    }
    /*check xor groups*/
    for( std::map<int, std::vector<int> >::iterator it =
           xorGroups.begin(); it != xorGroups.end(); ++it ) {
      std::vector<int> xorIDs = it->second;
      std::string xorstr;
      int xorNum = 0;
      for( i = 0; i < ( int )xorIDs.size(); i++ ) {
        if( groups[xorIDs[i]]->isSet ) {
          if(argc > 1)
            xorstr.append( " \"" ) \
            .append( groups[xorIDs[i]]->flags[0] ).append( "\" " );
          else
            xorstr.append(" \"") \
            .append(groups[xorIDs[i]]->help).append("\" ");
          xorNum++;
        }
      }
      if( xorNum > 1 ) {
        out.append( "ERROR:Can't set xor options at the same time:" ) \
        .append( xorstr ).append( "\n" );
        isValid  = false;
      }
    }
    /*print unknown OPTIONS*/
    if( ( int )unknownOptions.size() > 0 ) {
      for( i = 0; i < ( int )unknownOptions.size(); ++i ) {
        out.append( "WARNING: Got unknown argument:" ) \
        .append( unknownOptions[i] ).append( ".\n" );
      }
    }
    /*return out*/
    if( !out.empty() && !isValid && argc != 1 ) {
      out.append( "\n" ).append( getUsage() );
    }
    return isValid;
  };
protected:
  bool ok = false;
  bool open = true;
  int argc;
  const char **argv;
};