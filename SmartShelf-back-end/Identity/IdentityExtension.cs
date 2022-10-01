using System;
using System.Security.Claims;

namespace Identity
{
    public static class IdentityExtensions
    {
        public static int GetUserId(this ClaimsPrincipal principal)
        {
            return GetUserId<int>(principal);
        }

        public static TKey GetUserId<TKey>(this ClaimsPrincipal principal)
        {
            if (principal == null)
            {
                throw new ArgumentNullException(nameof(principal));
            }

            string userId = principal.FindFirst(ClaimTypes.NameIdentifier).Value;

            if (typeof(TKey) == typeof(int))
            {
                if (!int.TryParse(userId, out _))
                {
                    throw new ArgumentException("ClaimsPrincipal NameIdentifier is not of type int!");
                }
            }

            return (TKey)Convert.ChangeType(userId, typeof(TKey));

            // this is tiny bit slower, but handles GUID type also
            // return (TKey) TypeDescriptor.GetConverter(typeof(TKey)).ConvertFromInvariantString(userId);

        }
    }
}