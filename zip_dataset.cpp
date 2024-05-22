#include "header.h"
#include<tccore\item.h>



int  Dataset_zip(EPM_action_message_t msg)
{
	int ifail = 0;
	int iFail = ITK_ok;
	int argCount = 0;
	int iTargetAttachmentCount = 0;
	int iTargetSecondaryObjCount = 0;


	char* pcTargetObjType = NULL;
	char* pcTargetObjId = NULL;
	char* pcTargetSecondaryObjType = NULL;


	char** argKeys = NULL;
	char** argValues = NULL;


	tag_t tRelationType = NULLTAG;
	tag_t tRootTask = NULLTAG;


	tag_t* ptTargetAttachments = NULL;
	tag_t* ptTargetSecondaryObjTags = NULL;


	string szInclude_value;
	string szRelation_value;
	string szDataset_value;
	string szReference_value;
	string szDelimiter = ",";
	string szTargetObjId;
	string szFirstCmd;
	string szCommand;
	string szPath = "";
	string szTempLocation = "D:\\Apps\\teamcenter\\TC_ROOT\\temp\\";


	vector<string> include;
	size_t pos = 0;


	/*HANDLER ARGUMENTS*/
	EPM_args_process_args(msg.task, msg.arguments, &argCount, &argKeys, &argValues);
	for (int i = 0; i < argCount; i++)
	{
		if (tc_strcmp(argKeys[i], "Include_type") == 0)
		{
			szInclude_value = argValues[i];

		}
		if (tc_strcmp(argKeys[i], "Relation_type") == 0)
		{
			szRelation_value = argValues[i];

		}
		if (tc_strcmp(argKeys[i], "Dataset_type") == 0)
		{
			szDataset_value = argValues[i];

		}
		if (tc_strcmp(argKeys[i], "Reference_type") == 0)
		{
			szReference_value = argValues[i];

		}
	}



	/*TOKENIZING INCLUDE TYPE*/
	while ((pos = szInclude_value.find(szDelimiter)) != std::string::npos) {
		include.push_back(szInclude_value.substr(0, pos));
		szInclude_value.erase(0, pos + szDelimiter.length());
	}
	include.push_back(szInclude_value);



	/*ROOT TASK*/
	ifail = EPM_ask_root_task(msg.task, &tRootTask);
	if (ifail == 0 && tRootTask != NULLTAG)
	{   
		/*TARGET ATTACHMENTS*/
		EPM_ask_attachments(tRootTask, EPM_target_attachment, &iTargetAttachmentCount, &ptTargetAttachments);
		for (int i = 0; i < iTargetAttachmentCount; i++)
		{
			ifail = AOM_ask_value_string(ptTargetAttachments[i], "object_type", &pcTargetObjType);
			for (int j = 0; j < include.size(); j++)
			{
				if (tc_strcasecmp(pcTargetObjType, include[j].c_str()) == 0)
				{
					GRM_find_relation_type(szRelation_value.c_str(), &tRelationType);
					GRM_list_secondary_objects_only(ptTargetAttachments[i], tRelationType, &iTargetSecondaryObjCount, &ptTargetSecondaryObjTags);
					for (int k = 0; k < iTargetSecondaryObjCount; k++)
					{
						ifail = AOM_ask_value_string(ptTargetSecondaryObjTags[k], "object_type", &pcTargetSecondaryObjType);
						if (tc_strcasecmp(pcTargetSecondaryObjType, szDataset_value.c_str()) == 0)
						{
							ifail = AOM_ask_value_string(ptTargetAttachments[i], "item_id", &pcTargetObjId);
							szTargetObjId = pcTargetObjId;


							szPath = szTempLocation + szTargetObjId;
							szFirstCmd = "mkdir";
							szCommand = szFirstCmd + " " + szPath;
							ifail = system(szCommand.c_str());
							ifail = AE_export_named_ref(ptTargetSecondaryObjTags[k], szReference_value.c_str(), szPath.c_str());
						}
					}

				}
			}
		}
	}

	SAFE_SM_FREE(argKeys);
	SAFE_SM_FREE(argValues);
	SAFE_SM_FREE(ptTargetAttachments);
	SAFE_SM_FREE(pcTargetObjType);
	SAFE_SM_FREE(ptTargetSecondaryObjTags);
	SAFE_SM_FREE(pcTargetSecondaryObjType);
	SAFE_SM_FREE(pcTargetObjId);
	return ifail;
}

			

