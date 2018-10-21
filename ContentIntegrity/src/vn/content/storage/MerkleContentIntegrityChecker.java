package vn.content.storage;

import java.io.File;

import org.apache.commons.io.IOUtils;
import org.json.JSONArray;
import org.json.JSONObject;

import vn.content.util.AppContext;
import vn.content.util.PropertiesLoader;

public class MerkleContentIntegrityChecker {

	// number of file download
	private static int download = 0;
	// number of hash re-compute
	private static int recompute = 0;
	// number of hash compare
	private static int compare = 0;

	public MerkleContentIntegrityChecker() {
		// TODO Auto-generated constructor stub
	}

	public static void main(String[] args) {
		PropertiesLoader.loadPropertyFile();
		int numOfRun = 10;

		try {
			System.out.println("Create content inventory");
			NewInventoryCreation.inventoryCreation();
			for (int i = 0; i < numOfRun; i++) {
				System.out.println("Modify objects");
				OldInventoryCreation.inventoryCreation();
				System.out.println("Check content integrity");
				checkContentIntegrity();
			}
			double numOfCompare = ((double) compare) / (double) numOfRun;
			System.out.println("Average number of comparison: " + numOfCompare);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	private static void checkContentIntegrity() throws Exception {
		String localInventory = AppContext.getValue("vn.content.storage.inventory.old");
		String remoteInventory = AppContext.getValue("vn.content.storage.inventory.new");

		// load local inventory and retrieve the remote one
		String jsonTxt = IOUtils.toString(new File(localInventory).toURI(), "UTF-8");
		JSONObject jsonLI = new JSONObject(jsonTxt);
		jsonTxt = IOUtils.toString(new File(remoteInventory).toURI(), "UTF-8");
		JSONObject jsonRI = new JSONObject(jsonTxt);

		// content integrity check
		String localIH = jsonLI.getString("InventoryHash");
		String remoteIH = jsonRI.getString("InventoryHash");
		compare++;
		if (!localIH.equals(remoteIH)) {
			JSONArray remoteObjects = jsonRI.getJSONArray("Objects");
			JSONArray localObjects = jsonLI.getJSONArray("Objects");
			int numRemote = remoteObjects.length();
			int numLocal = localObjects.length();
			for (int i = 0; i < numRemote; i++) {
				JSONObject remoteObj = remoteObjects.getJSONObject(i);
				String remoteObjName = remoteObj.getString("ObjectName");
				String remoteObjHash = remoteObj.getString("ObjectHash");
				for (int j = 0; j < numLocal; j++) {
					JSONObject localObj = localObjects.getJSONObject(j);
					String localObjName = localObj.getString("ObjectName");
					String localObjHash = localObj.getString("ObjectHash");
					if (localObjName.equals(remoteObjName)) {
						compare++;
						if (!remoteObjHash.equals(localObjHash)) {
							// check object components
							String[] compNames = JSONObject.getNames(remoteObj);
							for (int k = 0; k < compNames.length; k++) {
								String compName = compNames[k];
								if (!compName.equals("ObjectName") && !compName.equals("ObjectHash")) {
									JSONObject remoteComp = remoteObj.getJSONObject(compName);
									JSONObject localComp = localObj.getJSONObject(compName);
									String remoteCompHash = remoteComp.getString("ComponentHash");
									String localCompHash = localComp.getString("ComponentHash");
									compare++;
									if (!remoteCompHash.equals(localCompHash)) {
										// check files in each component
										JSONArray remoteFS = remoteComp.getJSONArray("Files");
										JSONArray localFS = localComp.getJSONArray("Files");
										int numRFSLength = remoteFS.length();
										int numLFSLength = localFS.length();
										for (int a = 0; a < numRFSLength; a++) {
											JSONObject remoteFile = remoteFS.getJSONObject(a);
											String remoteFileName = remoteFile.getString("FileID");
											String remoteFileHash = remoteFile.getString("FileHash");
											for (int b = 0; b < numLFSLength; b++) {
												JSONObject localFile = localFS.getJSONObject(b);
												String localFileName = localFile.getString("FileID");
												String localFileHash = localFile.getString("FileHash");
												if (remoteFileName.equals(localFileName)) {
													compare++;
													if (!remoteFileHash.equals(localFileHash)) {
														download++;
													}
												}
											}
										}

									}
								}
							}
						}
					}
				}
			}

		} else {
			System.out.println("Content unchanged");
		}
	}

}
