package ab3_a7;

import java.util.ArrayList;

public interface IUser {

	public ISaltedHashEncryptonRSAMD5HardcoreSafe getPassword();

	public String firstName();

	public String lastName();

	public String email();

	public Integer id();

	public Integer created();

	public Integer phone();
	
	public ArrayList<String> paymentCredentials();

}
