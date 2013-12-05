package ab3_a7;

import java.sql.Date;

public interface IReservation {

	
	public int id();
	
	public IRoom room();
	
	public IUser user();
	
	public Date created();
	
	public boolean hasPaid();
	
	
}
