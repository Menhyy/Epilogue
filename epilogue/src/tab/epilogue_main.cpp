#include "../tools/writeFile.h"
#include "../tools/writeFileNet.h"
#include "tab/epilogue_main.hpp"

EpilogueMain::EpilogueMain()
{
    // Inflate the tab from the XML file
    this->inflateFromXMLRes("xml/tabs/epilogue.xml");

    // Bind the button click to a method using the macro (just for the sake of showcasing it, it's overkill in this situation)
    BRLS_REGISTER_CLICK_BY_ID("button_primary", this->onPrimaryButtonClicked);
    BRLS_REGISTER_CLICK_BY_ID("button_net", this->onNetButtonClicked);
}

bool EpilogueMain::onNetButtonClicked(brls::View* view)
{
    const std::string result = writeFileDebugNet("PLACEHOLDER", "PLACEHOLDER", "sdmc:/TestFileNet.txt");
    brls::Logger::info("Net Button Clicked");
    if (result  == "CURL_ERR")
    {
        brls::Application::notify("Net File Error!!!\nFallback written to file.\nContents:" + result);
    }else
    {
        brls::Application::notify("Network file written successfully!\nContents:\n" + result);
    }
    return true;
}

bool EpilogueMain::onPrimaryButtonClicked(brls::View* view)
{
    brls::Logger::info("Clicked");
    if (writeFileDebug())
    {
        brls::Application::notify("Test File Written!");
    }
    return true;
}

brls::View* EpilogueMain::create()
{
    return new EpilogueMain();
}
