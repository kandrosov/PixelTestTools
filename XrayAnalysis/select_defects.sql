SET SESSION group_concat_max_len = 1000000;
SELECT  sessions_bare.center AS center_bare,
        sessions_xray.center AS center_xray,
        inventory_fullmodule.fullmodule_id,
        inventory_fullmodule.baremodule_id,
        GREATEST(view12.max_fq_grade, view12.xr_grade) AS final_grade,
        CONCAT('{', COALESCE(GROUP_CONCAT(NULLIF(
            SUBSTR(XRay_ROC.addr_pixels_bad, 2, CHAR_LENGTH(XRay_ROC.addr_pixels_bad) - 2), '')), ''),
            '}') AS xray_failures,
        COALESCE(NULLIF(qa_bonding.failures, ''), '{}') AS bb_failures,
        COALESCE(NULLIF(qa_pixelalive.failures, ''), '{}') AS pa_failures
FROM Test_FullModule_XRay_HR_Roc_Analysis_Summary XRay_ROC
LEFT OUTER JOIN Test_FullModule_XRay_HR_Roc_Analysis_Summary XRay_ROC_2
    ON XRay_ROC.test_xray_hr_summary_id = XRay_ROC_2.test_xray_hr_summary_id
    AND XRay_ROC.processing_id < XRay_ROC_2.processing_id
INNER JOIN Test_FullModule_XRay_HR_Summary
    ON Test_FullModule_XRay_HR_Summary.test_id = XRay_ROC.test_xray_hr_summary_id
INNER JOIN inventory_fullmodule ON Test_FullModule_XRay_HR_Summary.fullmodule_id = inventory_fullmodule.fullmodule_id
INNER JOIN inventory_baremodule ON inventory_baremodule.baremodule_id = inventory_fullmodule.baremodule_id
INNER JOIN test_baremodule_qa qa_bonding ON qa_bonding.test_id = inventory_baremodule.lasttest_baremodule_qa_bonding
INNER JOIN test_baremodule_qa qa_pixelalive
    ON qa_pixelalive.test_id = inventory_baremodule.lasttest_baremodule_qa_pixelalive
INNER JOIN view12
    ON view12.fm_fullmodule_id = inventory_fullmodule.fullmodule_id
INNER JOIN sessions sessions_xray ON sessions_xray.session_id = XRay_ROC.session_id
INNER JOIN sessions sessions_bare ON sessions_bare.session_id = qa_pixelalive.session_id
WHERE XRay_ROC_2.test_id IS NULL
GROUP BY Test_FullModule_XRay_HR_Summary.test_id;
