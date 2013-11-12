
DROP TABLE fpa.agg_fpa_fact_test_2;
CREATE TABLE fpa.agg_fact_test_2 (
  geo_dest_key varchar,
  acquisition varchar,
  stage varchar,
  bookings float8,
  fees_before_cancellations float8,
  revenue float8,
  trips_ended float8
  ratings_0s  float8,
  ratings_1s  float8,
  ratings_2s  float8,
  ratings_3s  float8,
  ratings_4s  float8,
  ratings_5s  float8,
  active_listings float8,
  admin_cancellations float8,
  contacters  float8,
  contacts  float8,
  coupon_contra_revenue float8,
  coupon_contra_revenue_before_cancellations  float8,
  coupon_losses float8,
  coupon_losses_before_cancellations  float8,
  deactivated float8,
  fx_fee_revenue  float8,
  fx_fees_before_cancellations  float8,
  guest_cancellations float8,
  guest_fee_revenue float8,
  guest_fees_before_cancellations float8,
  host_cancellations  float8,
  host_fee_revenue  float8,
  host_fees_before_cancellations  float8,
  n_5_star_reviews  float8,
  n_trips_comp_reviews  float8,
  new_active  float8,
  new_listings  float8,
  nights  float8,
  price float8,
  reactivated float8,
  searchers float8,
  searchers_omg float8,
  signups float8,
  trips float8,
  trips_aborted_by_admin  float8,
  trips_aborted_by_guest  float8,
  trips_aborted_by_host float8,
  fact_count int
)

INSERT INTO fpa.agg_fact_test_2(
  geo_dest_key,
  acquisition,
  stage,
  bookings,
  fees_before_cancellations,
  revenue,
  trips_ended,
  ratings_0s,
  ratings_1s,
  ratings_2s,
  ratings_3s,
  ratings_4s,
  ratings_5s,
  active_listings,
  admin_cancellations,
  contacters,
  contacts,
  coupon_contra_revenue,
  coupon_contra_revenue_before_cancellations,
  coupon_losses,
  coupon_losses_before_cancellations,
  deactivated,
  fx_fee_revenue,
  fx_fees_before_cancellations,
  guest_cancellations,
  guest_fee_revenue,
  guest_fees_before_cancellations,
  host_cancellations,
  host_fee_revenue,
  host_fees_before_cancellations,
  n_5_star_reviews,
  n_trips_comp_reviews,
  new_active,
  new_listings,
  nights,
  price,
  reactivated,
  searchers,
  searchers_omg,
  signups,
  trips,
  trips_aborted_by_admin,
  trips_aborted_by_guest,
  trips_aborted_by_host,
  fact_count)
SELECT
  geo_dest_key,
  sum(acquisition)AS acquisition,
  sum(stage) AS stage,
  sum(bookings) AS bookings,
  sum(fees_before_cancellations) AS fees_before_cancellations,
  sum(revenue) AS revenue,
  sum(trips_ended) AS trips_ended,
  sum(ratings_4s) AS ratings_0s,
  sum(ratings_4s) AS ratings_1s,
  sum(ratings_4s) AS ratings_2s,
  sum(ratings_4s) AS ratings_3s,
  sum(ratings_4s) AS ratings_4s,
  sum(ratings_5s) AS ratings_5s,
  sum(active_listings) AS active_listings,
  sum(admin_cancellations) AS admin_cancellations,
  sum(contacters) AS contacters,
  sum(contacts) AS contacts,
  sum(coupon_contra_revenue) AS coupon_contra_revenue,
  sum(coupon_contra_revenue_before_cancellations) AS coupon_contra_revenue_before_cancellations,
  sum(coupon_losses) AS coupon_losses,
  sum(coupon_losses_before_cancellations) AS coupon_losses_before_cancellations,
  sum(deactivated) AS deactivated,
  sum(fx_fee_revenue) AS fx_fee_revenue
  sum(fx_fees_before_cancellations) AS fx_fees_before_cancellations,
  sum(guest_cancellations) AS guest_cancellations,
  sum(guest_fee_revenue) AS guest_fee_revenue,
  sum(guest_fees_before_cancellations) AS guest_fees_before_cancellations,
  sum(host_cancellations) AS host_cancellations,
  sum(host_fee_revenue) AS host_fee_revenue,
  sum(host_fees_before_cancellations) AS host_fees_before_cancellations,
FROM
  fpa.fact_test_2
GROUP BY
  geo_dest_key
