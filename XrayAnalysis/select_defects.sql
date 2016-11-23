SET SESSION group_concat_max_len = 1000000;
SELECT  sessions.center,
        inventory_fullmodule.fullmodule_id,
        inventory_fullmodule.baremodule_id,
        CONCAT('{',
            GROUP_CONCAT(NULLIF(SUBSTR(XRay_ROC.addr_pixels_bad, 2, CHAR_LENGTH(XRay_ROC.addr_pixels_bad) - 2), '')),
            '}') as xray_failures,
        qa_bonding.failures as bb_failures,
        qa_pixelalive.failures as pa_failures
FROM Test_FullModule_XRay_HR_Roc_Analysis_Summary XRay_ROC
LEFT OUTER JOIN Test_FullModule_XRay_HR_Roc_Analysis_Summary XRay_ROC_2
    ON XRay_ROC.test_xray_hr_summary_id = XRay_ROC_2.test_xray_hr_summary_id
    AND XRay_ROC.processing_id < XRay_ROC_2.processing_id
INNER JOIN Test_FullModule_XRay_HR_Summary
    ON Test_FullModule_XRay_HR_Summary.test_id = XRay_ROC.test_xray_hr_summary_id
INNER JOIN sessions ON sessions.session_id = XRay_ROC.session_id
INNER JOIN inventory_fullmodule ON Test_FullModule_XRay_HR_Summary.fullmodule_id = inventory_fullmodule.fullmodule_id
INNER JOIN inventory_baremodule ON inventory_baremodule.baremodule_id = inventory_fullmodule.baremodule_id
INNER JOIN test_baremodule_qa qa_bonding ON qa_bonding.test_id = inventory_baremodule.lasttest_baremodule_qa_bonding
INNER JOIN test_baremodule_qa qa_pixelalive
    ON qa_pixelalive.test_id = inventory_baremodule.lasttest_baremodule_qa_pixelalive
WHERE XRay_ROC_2.test_id IS NULL
GROUP BY Test_FullModule_XRay_HR_Summary.test_id;
