////
//
// Imports
//
////

use frame_support::{
  dispatch::{
    DispatchResultWithPostInfo
  },

  inherent::{
    Vec
  },

  sp_runtime::{
    DispatchError
  }
};

pub trait UniqueAssets<AccountId>
{
  type AssetId;
  type AssetInfo;

  fn assets_for_account(account: &AccountId) -> Vec<Self::AssetId>;
  fn account_for_asset(asset_id: &Self::AssetId) -> AccountId;

  fn licenses_for_account(account: &AccountId) -> Vec<Self::AssetId>;
  fn licenses_for_asset(asset_id: &Self::AssetId) -> Vec<AccountId>;

  fn create(
    owner: &AccountId,
    owner_asset_info: Self::AssetInfo,
  ) -> Result<Self::AssetId, DispatchError>;

  fn destroy(
    asset_id: &Self::AssetId
  ) -> DispatchResultWithPostInfo;

  fn license(
    target: &AccountId,
    target_asset_id: &Self::AssetId
  ) -> DispatchResultWithPostInfo;

  fn transfer(
    target: &AccountId,
    target_asset_id: &Self::AssetId
  ) -> DispatchResultWithPostInfo;
  
}
