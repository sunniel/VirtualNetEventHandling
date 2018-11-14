package vn.content.storage;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

import org.json.JSONArray;
import org.json.JSONObject;

import vn.content.util.AppContext;
import vn.content.util.CryptoUtil;
import vn.content.util.PropertiesLoader;

public class OldInventoryCreation {
	public OldInventoryCreation() {
		// TODO Auto-generated constructor stub
	}

	public static void main(String[] args) {
		PropertiesLoader.loadPropertyFile();

		try {
			System.out.println("Create a modified content inventory");
			inventoryCreation();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public static void inventoryCreation() throws Exception {
		String path = AppContext.getValue("vn.content.storage.content");
		int changes = Integer.valueOf(AppContext.getValue("vn.content.storage.content.change")).intValue();
		Integer fileIndex = 0;
		String baseHash = CryptoUtil.hash("This is the base hash");
		Random r = new Random();
		File root = new File(path);
		File[] objects = root.listFiles();
		JSONObject inventory = new JSONObject();
		JSONArray jObjects = new JSONArray();
		// select the number of changed files
		int nof = Long.valueOf(fileCount(Paths.get(path))).intValue();
		ArrayList<Integer> changeIndex = new ArrayList<Integer>();
		for (int i = 0; i < changes; i++) {
			Integer index = r.nextInt(nof);
			while (changeIndex.contains(index)) {
				index = r.nextInt(nof);
			}
			changeIndex.add(index);
		}

		// create the inventory
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
					String hash = null;
					// randomly change a file (in file hash)
					if (changeIndex.contains(fileIndex)) {
						hash = generateFileHash(baseHash, comp.getPath());
					} else {
						hash = generateFileHash(comp.getPath());
					}
					fileIndex++;
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
						String hash = null;
						// randomly change a file (in file hash)
						if (changeIndex.contains(fileIndex)) {
							hash = generateFileHash(baseHash, file.getPath());
						} else {
							hash = generateFileHash(file.getPath());
						}
						fileIndex++;
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
		String inventoryFile = AppContext.getValue("vn.content.storage.inventory.old");
		PrintWriter writer = new PrintWriter(new FileWriter(inventoryFile, false));
		writer.println(inventory.toString(4));
		writer.flush();
		writer.close();
	}

	public static long fileCount(Path dir) throws Exception {
		return Files.walk(dir).parallel().filter(p -> !p.toFile().isDirectory()).count();
	}

	private static String generateFileHash(String path) throws Exception {
		try {
			return CryptoUtil.generateFileDigestHex(path);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			throw e;
		}
	}

	private static String generateFileHash(String baseHash, String path) throws Exception {
		try {
			return CryptoUtil.generateFileDigestHex(baseHash, path);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			throw e;
		}
	}

}
