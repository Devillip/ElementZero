#include <Actor/Player.h>
#include <Math/BlockPos.h>
#include <Net/NetworkIdentifier.h>
#include <Packet/PlayerActionPacket.h>
#include <Packet/InventoryTransactionPacket.h>

#include <dllentry.h>
#include <hook.h>
#include <log.h>
#include <playerdb.h>
#include <command.h>
#include <audit.h>

#include "settings.h"

Settings settings;
Mode mode;

DEFAULT_SETTINGS(settings);

void dllenter() {}
void dllexit() {}

void checkAction(Mod::PlayerEntry const &, Mod::PlayerAction const &, Mod::CallbackToken<std::string> &);
void checkInventoryTransaction(
    Mod::PlayerEntry const &, ComplexInventoryTransaction const &, Mod::CallbackToken<std::string> &);

void PreInit() {
  mode = settings.AllowOperator ? Mode::Permissive : Mode::Enforce;
  Mod::CommandSupport::GetInstance().AddListener(SIG("loaded"), initCommand);
  Mod::AuditSystem::GetInstance().AddListener(SIG("action"), checkAction);
  Mod::AuditSystem::GetInstance().AddListener(SIG("inventory_transaction"), checkInventoryTransaction);
}

static bool Check(Player *player, int x, int z) {
  if (mode == Mode::Disabled) return true;
  if (x < settings.Start.x || x > settings.End.x) return true;
  if (z < settings.Start.z || z > settings.End.z) return true;
  if (player->getCommandPermissionLevel() != CommandPermissionLevel::Normal && mode == Mode::Permissive) return true;
  return false;
}

void checkAction(
    Mod::PlayerEntry const &entry, Mod::PlayerAction const &action, Mod::CallbackToken<std::string> &token) {
  DEF_LOGGER("debug");
  if (entry.player->getDimensionId() == 0) {
    switch (action.type) {
    case PlayerActionType::START_BREAK:
    case PlayerActionType::CONTINUE_BREAK:
    case PlayerActionType::INTERACT_BLOCK:
      if (!Check(entry.player, action.pos.x, action.pos.z)) token("Blocked by SpawnProtection");
    default: break;
    }
  }
}
void checkInventoryTransaction(
    Mod::PlayerEntry const &entry, ComplexInventoryTransaction const &transaction,
    Mod::CallbackToken<std::string> &token) {
  if (entry.player->getDimensionId() == 0) {
    switch (transaction.type) {
    case ComplexInventoryTransaction::Type::ITEM_USE: {
      auto &data    = (ItemUseInventoryTransaction const &) transaction;
      auto composed = data.playerPos + data.clickPos;
      if (!Check(entry.player, composed.x, composed.z)) {
        data.onTransactionError(*entry.player, InventoryTransactionError::Unexcepted);
        token("Blocked by SpawnProtection");
      }
    } break;
    case ComplexInventoryTransaction::Type::ITEM_USE_ON_ACTOR: {
      auto &data    = (ItemUseOnActorInventoryTransaction const &) transaction;
      auto composed = data.playerPos + data.clickPos;
      if (!Check(entry.player, composed.x, composed.z)) {
        data.onTransactionError(*entry.player, InventoryTransactionError::Unexcepted);
        token("Blocked by SpawnProtection");
      }
    } break;
    default: break;
    }
  }
}