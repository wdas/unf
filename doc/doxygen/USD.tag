<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="namespace">
    <name>PXR_NS</name>
    <filename></filename>
    <class kind="class">PXR_NS::TfNotice</class>
    <class kind="class">PXR_NS::TfType</class>
    <class kind="class">PXR_NS::TfRefBase</class>
    <class kind="class">PXR_NS::TfRefPtr</class>
    <class kind="class">PXR_NS::TfWeakBase</class>
    <class kind="class">PXR_NS::TfWeakPtr</class>
    <class kind="class">PXR_NS::TfAnyWeakPtr</class>
    <class kind="class">PXR_NS::TfToken</class>
    <class kind="class">PXR_NS::SdfPath</class>
    <class kind="class">PXR_NS::UsdObject</class>
    <class kind="class">PXR_NS::UsdStage</class>
    <class kind="class">PXR_NS::UsdStageRefPtr</class>
    <class kind="class">PXR_NS::UsdStageWeakPtr</class>
    <class kind="class">PXR_NS::UsdNotice</class>
  </compound>
  <compound kind="class">
    <name>PXR_NS::TfNotice</name>
    <filename>class_tf_notice.html</filename>
    <class kind="class">PXR_NS::TfType::FactoryBase</class>
  </compound>
  <compound kind="class">
    <name>PXR_NS::TfNotice::Key</name>
    <filename>class_tf_notice_1_1_key.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::TfType</name>
    <filename>class_tf_type.html</filename>
    <class kind="class">PXR_NS::TfType::FactoryBase</class>
  </compound>
  <compound kind="class">
    <name>PXR_NS::TfType::FactoryBase</name>
    <filename>class_tf_type_1_1_factory_base.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::TfRefBase</name>
    <filename>class_tf_ref_base.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::TfRefPtr</name>
    <filename>class_tf_ref_ptr.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::TfWeakBase</name>
    <filename>class_tf_weak_base.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::TfWeakPtr</name>
    <filename>class_tf_weak_ptr.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::TfAnyWeakPtr</name>
    <filename>class_tf_any_weak_ptr.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::TfToken</name>
    <filename>class_tf_token.html</filename>
    <class kind="class">PXR_NS::TfToken::HashFunctor</class>
  </compound>
  <compound kind="class">
    <name>PXR_NS::TfToken::HashFunctor</name>
    <filename>struct_tf_token_1_1_hash_functor.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::SdfPath</name>
    <filename>class_sdf_path.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::UsdObject</name>
    <filename>class_usd_object.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::UsdStage</name>
    <filename>class_usd_stage.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::UsdStageRefPtr</name>
    <filename>class_usd_stage.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::UsdStageWeakPtr</name>
    <filename>class_usd_stage.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::UsdNotice</name>
    <filename>class_usd_notice.html</filename>
    <class kind="class">PXR_NS::UsdNotice::StageNotice</class>
    <class kind="class">PXR_NS::UsdNotice::LayerMutingChanged</class>
    <class kind="class">PXR_NS::UsdNotice::ObjectsChanged</class>
    <class kind="class">PXR_NS::UsdNotice::StageContentsChanged</class>
    <class kind="class">PXR_NS::UsdNotice::StageEditTargetChanged</class>
  </compound>
  <compound kind="class">
    <name>PXR_NS::UsdNotice::StageNotice</name>
    <filename>class_usd_notice_1_1_stage_notice.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::UsdNotice::LayerMutingChanged</name>
    <filename>class_usd_notice_1_1_layer_muting_changed.html</filename>
    <member kind="function">
      <name>GetMutedLayers</name>
      <anchor>a46081415d09be1ff2b028fd18f78ef16</anchor>
    </member>
    <member kind="function">
      <name>GetUnmutedLayers</name>
      <anchor>afbb387a15c8d7e1441cb4b962999aa07</anchor>
    </member>
  </compound>
  <compound kind="class">
    <name>PXR_NS::UsdNotice::ObjectsChanged</name>
    <filename>class_usd_notice_1_1_objects_changed.html</filename>
    <member kind="function">
      <name>AffectedObject</name>
      <anchor>a3c090cd8e87d62dfabf10555d6c7db13</anchor>
    </member>
    <member kind="function">
      <name>ResyncedObject</name>
      <anchor>abb3e5a33dc3dadb89d449d05af55cb30</anchor>
    </member>
    <member kind="function">
      <name>ChangedInfoOnly</name>
      <anchor>ab939047696102fb8ff522b5bbcaeb274</anchor>
    </member>
    <member kind="function">
      <name>GetResyncedPaths</name>
      <anchor>a793e3d781e6e01889a0ba8d789c09102</anchor>
    </member>
    <member kind="function">
      <name>GetChangedInfoOnlyPaths</name>
      <anchor>a235ad194cc01a9a16dabc0abe9bed144</anchor>
    </member>
    <member kind="function">
      <name>GetChangedFields</name>
      <anchor>a635f91971271c3fead108b2288fb0781</anchor>
      <arglist>(const UsdObject&amp;) const</arglist>
    </member>
    <member kind="function">
      <name>GetChangedFields</name>
      <anchor>a6a4d7574081752d2ecc6c0aaa31d4e50</anchor>
      <arglist>(const SdfPath&amp;) const</arglist>
    </member>
    <member kind="function">
      <name>HasChangedFields</name>
      <anchor>a8c1cac8459f107fde54042a657aaf440</anchor>
      <arglist>(const UsdObject&amp;) const</arglist>
    </member>
    <member kind="function">
      <name>HasChangedFields</name>
      <anchor>ab10bc8abddba1c9eb3c7f864636ec1d2</anchor>
      <arglist>(const SdfPath&amp;) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PXR_NS::UsdNotice::StageContentsChanged</name>
    <filename>class_usd_notice_1_1_stage_contents_changed.html</filename>
  </compound>
  <compound kind="class">
    <name>PXR_NS::UsdNotice::StageEditTargetChanged</name>
    <filename>class_usd_notice_1_1_stage_edit_target_changed.html</filename>
  </compound>
</tagfile>
