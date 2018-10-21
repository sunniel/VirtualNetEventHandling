package vn.content.util;

import java.io.File;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.DigestInputStream;
import java.security.MessageDigest;

import org.apache.commons.codec.binary.Hex;
import org.apache.commons.codec.digest.DigestUtils;

public class CryptoUtil {

	public static String generateFileDigestHex(String path) throws Exception {
		MessageDigest md = MessageDigest.getInstance("MD5");
		File file = new File(path);
		int size = (int) file.length();
		try {
			InputStream is = Files.newInputStream(Paths.get(path));
			DigestInputStream dis = new DigestInputStream(is, md);
			/* Read decorated stream (dis) to EOF as normal... */
			byte[] buffer = new byte[size];
			while (dis.read(buffer) > -1) {
			}
			MessageDigest digest = dis.getMessageDigest();
			dis.close();
			byte[] digestByte = digest.digest();
			// to hex string
			String digestHex = Hex.encodeHexString(digestByte);
			return digestHex;
		} catch (

		Exception e) {
			e.printStackTrace();
			throw e;
		}
	}

	public static String generateFileDigestHex(String previousHash, String path) throws Exception {
		MessageDigest md = MessageDigest.getInstance("MD5");
		File file = new File(path);
		int size = (int) file.length();
		try {
			InputStream is = Files.newInputStream(Paths.get(path));
			DigestInputStream dis = new DigestInputStream(is, md);
			/* Read decorated stream (dis) to EOF as normal... */
			byte[] buffer = new byte[size];
			while (dis.read(buffer) > -1) {
			}
			MessageDigest digest = dis.getMessageDigest();
			dis.close();
			byte[] digestByte = digest.digest();
			// to hex string
			String digestHex = Hex.encodeHexString(digestByte);
			String chainedHash = digestHex;
			if (previousHash != null) {
				chainedHash = hash(previousHash + digestHex);
			}
			return chainedHash;
		} catch (Exception e) {
			e.printStackTrace();
			throw e;
		}
	}

	public static String hash(String content) {
		return DigestUtils.md5Hex(content);
	}
}
