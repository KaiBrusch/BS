package ab3_a7;

import java.util.ArrayList;

public interface IPayment extends BrainTree {

	public ArrayList<String> paymentInformation();

	public double paymentAmount();
}
