package vn.content.storage;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.Map;

import org.json.JSONArray;
import org.json.JSONObject;

import vn.content.util.AppContext;
import vn.content.util.CryptoUtil;
import vn.content.util.PropertiesLoader;

public class NewInventoryCreation {

	public NewInventoryCreation() {
		// TODO Auto-generated constructor stub
	}

	public static void main(String[] args) {
		PropertiesLoader.loadPropertyFile();

		try {
			System.out.println("Create a content inventory");
			inventoryCreation();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public static void inventoryCreation() throws Exception {
		String path = AppContext.getValue("vn.content.storage.content");
		File root = new File(path);
		File[] objects = root.listFiles();
		JSONObject inventory = new JSONObject();
		JSONArray jObjects = new JSONArray();
		String inventoryHash = "";
		for (File object : objects) {
			JSONObject jObject = new JSONObject();
			String objectName = object.getName();
			jObject.put("ObjectName", objectName);
			JSONObject jModels = new JSONObject();
			JSONArray jModelFiles = new JSONArray();
			File[] comps = object.listFiles();
			Map<String, String> compHashes = new HashMap<String, String>();
			String modelHash = "";
			for (File comp : comps) {
				if (comp.isFile()) {
					String fileName = comp.getName();
					String hash = generateFileHash(comp.getPath());
					JSONObject file = new JSONObject();
					file.put("FileID", fileName);
					file.put("FileHash", hash);
					modelHash = CryptoUtil.hash(modelHash + hash);
					jModelFiles.put(file);
				} else {
					String compName = comp.getName();
					JSONObject jComp = new JSONObject();
					JSONArray jFiles = new JSONArray();
					File[] files = comp.listFiles();
					String compHash = "";
					for (File file : files) {
						String fileName = file.getName();
						String hash = generateFileHash(file.getPath());
						JSONObject compFile = new JSONObject();
						compFile.put("FileID", fileName);
						compFile.put("FileHash", hash);
						compHash = CryptoUtil.hash(compHash + hash);
						jFiles.put(compFile);
					}
					jComp.put("Files", jFiles);
					jComp.put("ComponentHash", compHash);
					compHashes.put(compName, compHash);
					jObject.put(compName, jComp);
				}
			}
			jModels.put("Files", jModelFiles);
			jModels.put("ComponentHash", modelHash);
			compHashes.put("Models", modelHash);
			jObject.put("Models", jModels);

			String objectHash = "";
			for (Map.Entry<String, String> fileHash : compHashes.entrySet()) {
				objectHash = CryptoUtil.hash(objectHash + fileHash.getValue());
			}
			jObject.put("ObjectHash", objectHash);
			jObjects.put(jObject);
			inventoryHash = CryptoUtil.hash(inventoryHash + objectHash);
		}
		inventory.put("Objects", jObjects);
		inventory.put("InventoryHash", inventoryHash);

		// store the inventory to a JSON file
		String inventoryFile = AppContext.getValue("vn.content.storage.inventory.new");
		PrintWriter writer = new PrintWriter(new FileWriter(inventoryFile, false));
		writer.println(inventory.toString(4));
		writer.flush();
		writer.close();
	}

	private static String generateFileHash(String path) throws Exception {
		try {
			return CryptoUtil.generateFileDigestHex(path);
		} catch (Exception e) {
			e.printStackTrace();
			throw e;
		}
	}

}
