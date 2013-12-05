package ab3_a7;

public interface IMitarbeiterFassade {

	public boolean createRequest(int id);

	public boolean createUser(IUser user);

	public boolean deleteUser(IUser user);

	public boolean updateUser(IUser user);

	public boolean viewUser(int id, IUser user, String searchCriteria);

	public boolean updateRoom(IRoom room);

	public boolean insertRoom(IRoom room);

	public boolean deleteRoom(IRoom room);

}
