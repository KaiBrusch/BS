package ab3_a7;

import java.util.ArrayList;
import java.util.Date;
import java.util.concurrent.ConcurrentSkipListSet;

/*
 * Everything works Monadic. IO Operations are catched by Nothing or Just() Values that wrap around
 * any nulltype or wildcard value. The standard feedback to the screen would be "Something went wrong - 404"
 * and the option to go back to the previous last successful action.
 * 
 * Usage:
 * 	Login(getUser());
 * 	searchRom(IO()); 
 * 	finishReservation(
 * 		createReservation(IO());
 * 		<= asynchronous process => 
 * 		paymentProcessing(IO());
 * 	);
 */

public interface IHotelsystemFassade {

	/*
	 * Logins the user into the system if the user is valid.
	 * 
	 * @param user The user as Interface IUser.
	 * 
	 * @return boolean if the user successfully logged in.
	 * 
	 * @throw User exists ->? Throws no_user_of_that_specific_name_exception
	 * 
	 * @throw User password ->? Throws user_has_no_good_password_exception
	 * 
	 * @pre User must be registered with an email
	 * 
	 * @post you're logged in!
	 */
	public boolean Login(IUser user);

	/*
	 * Searches the database for rooms and returns all rooms that are free and
	 * fits the total amount of guests that are staying from the start date to
	 * the final date.
	 * 
	 * @param from start date of the stay.
	 * 
	 * @param to final date of the stay.
	 * 
	 * @param amountOfGuests final date of the stay. Default value is 1
	 * 
	 * @param optionalCriteria is a list of optional filters to narrow down the
	 * search. Default value is an empty List.
	 * 
	 * @return ConcurrentSkipListSet<IRoom> a list of rooms available.
	 * 
	 * @pre is date correct from < to && to > today if not give a feedback
	 * 
	 * @throw Room not available ->? give a feedback that no rooms are available
	 * to this date
	 * 
	 * @post user has list of rooms on the screen which he can choose from
	 */
	public ConcurrentSkipListSet<IRoom> searchRoom(Date from, Date to,
			int amountOfGuests/* =1 */, ArrayList<String> optionalCriteria/* =[] */);

	/*
	 * Creates a reservation from date intervall, a chosen room and the amount
	 * of guests. The reservation is a asynchronous process that will ask for
	 * payment later on.
	 * 
	 * @param from start date of the stay.
	 * 
	 * @param to final date of the stay.
	 * 
	 * @param amountOfGuests amount of guests that will stay with the user
	 * 
	 * @param room the chosen room
	 * 
	 * @param amountOfGuests amount of guests that will stay with the user
	 * 
	 * @return is a reservation interface.
	 * 
	 * @throw Date Combination is correct :: from < to && to > today ->? Throws
	 * date_range_out_of_bounds_exception
	 * 
	 * @throw the amount of guests has to be below or equal to the beds in the
	 * room :: amountOfGuests > room.beds ->? Throws
	 * number_of_beds_out_bounds_exception
	 * 
	 * @post reservation is set. User gets feedback.
	 */
	public IReservation createReservation(Date from, Date to, IUser user,
			IRoom room, int amountOfGuests);

	/*
	 * The payment needs its payment informations and the amount to be paid in
	 * it's currency. This is handled by BrainTree, doing some great magic with
	 * payment processing. It returns an object by the interfaceType IPayment
	 * which holds all the necessary information
	 * 
	 * @param paymentAmount - the payment for the whole Reservation in Euro.
	 * 
	 * @param IUser user - the current user that makes the payment. Has a method
	 * with his credntials. If he has none, ask for it.
	 * 
	 * @return IPayment interface that is connected to the reservation
	 * 
	 * @throw reservation exists ->? Throws no_reservation_found_exception
	 * 
	 * @throw payment is not valid ->? Throws payment_invalid_exception
	 * 
	 * @post user has reservationID and reservation status is done, user gets
	 * email
	 */
	public IPayment paymentProcessing(IUser user, double paymentAmount);

	/*
	 * Finishes the current reservation by giving back a reservationId and sets
	 * several database values on finished or processed. It also resolves the
	 * payment processing and the createReservation Transaction.
	 * 
	 * @param reservation - the current reservation object
	 * 
	 * @param payment - current payment information
	 * 
	 * @return int returns the reservationID
	 * 
	 * @throw reservation is valid ->? Throws reservation_not_valid_exception
	 * 
	 * @throw payment is valid ->? Throws payment_not_valid_exception
	 * 
	 * @post user has reservationID and reservation status is done, user gets
	 * email
	 */
	public int finishReservation(IReservation reservation, IPayment payment);

}
