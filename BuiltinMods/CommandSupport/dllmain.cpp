#include <command.h>
#include <hook.h>
#include <dllentry.h>

void (Mod::CommandSupport::*emitter)(sigt<"loaded"_sig>, CommandRegistry *);

Mod::CommandSupport::CommandSupport() { emitter = &Mod::CommandSupport::Emit; }

Mod::CommandSupport &Mod::CommandSupport::GetInstance() {
  static CommandSupport instance;
  return instance;
}

THook(void, "?setup@ChangeSettingCommand@@SAXAEAVCommandRegistry@@@Z", CommandRegistry *registry) {
  original(registry);
  (Mod::CommandSupport::GetInstance().*emitter)(SIG("loaded"), registry);
}

short &Mod::CommandSupport::type_id_count() {
  static auto ptr = (short *) GetServerSymbol("?count@?$typeid_t@VCommandRegistry@@@@2GA");
  return *ptr;
}

template <std::uint32_t> inline static typeid_t<CommandRegistry> getid(char const *name) {
  static auto ptr = (typeid_t<CommandRegistry> *) GetServerSymbol(name);
  return *ptr;
}

#define GETID(name) getid<name##_sig>(name)

template <> typeid_t<CommandRegistry> Mod::CommandSupport::GetParameterTypeId<bool>() {
  return GETID("?id@?1???$type_id@VCommandRegistry@@_N@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <> typeid_t<CommandRegistry> Mod::CommandSupport::GetParameterTypeId<int>() {
  return GETID("?id@?1???$type_id@VCommandRegistry@@H@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <> typeid_t<CommandRegistry> Mod::CommandSupport::GetParameterTypeId<float>() {
  return GETID("?id@?1???$type_id@VCommandRegistry@@M@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <> typeid_t<CommandRegistry> Mod::CommandSupport::GetParameterTypeId<std::string>() {
  return GETID(
      "?id@?1???$type_id@VCommandRegistry@@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@@YA?AV?$"
      "typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <> typeid_t<CommandRegistry> Mod::CommandSupport::GetParameterTypeId<CommandItem>() {
  return GETID("?id@?1???$type_id@VCommandRegistry@@VCommandItem@@@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <> typeid_t<CommandRegistry> Mod::CommandSupport::GetParameterTypeId<Json::Value>() {
  return GETID("?id@?1???$type_id@VCommandRegistry@@VValue@Json@@@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@A");
}
template <> typeid_t<CommandRegistry> Mod::CommandSupport::GetParameterTypeId<CommandSelector<Actor>>() {
  return GETID(
      "?id@?1???$type_id@VCommandRegistry@@V?$CommandSelector@VActor@@@@@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@"
      "A");
}
template <> typeid_t<CommandRegistry> Mod::CommandSupport::GetParameterTypeId<CommandSelector<Player>>() {
  return GETID(
      "?id@?1???$type_id@VCommandRegistry@@V?$CommandSelector@VPlayer@@@@@@YA?AV?$typeid_t@VCommandRegistry@@@@XZ@4V1@"
      "A");
}

void dllenter() {}
void dllexit() {}