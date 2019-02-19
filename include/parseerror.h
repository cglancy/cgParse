/**
* Copyright 2017 Charles Glancy (charles@glancyfamily.net)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without restriction, including  without limitation the rights to use, copy,
* modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
* is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once

namespace cg
{
    enum ParseError
    {
        UnknownError = -1,
        OtherCause = -1,
        NoError = 0,
        InternalServerError = 1,
        ServiceUnavailable = 2,
        ClientDisconnected = 4,
        ConnectionFailed = 100,
        UserInvalidLoginParams = 101,
        ObjectNotFound = 101,
        InvalidQuery = 102,
        InvalidClassName = 103,
        MissingObjectId = 104,
        InvalidFieldName = 105,
        InvalidPointer = 106,
        InvalidJSON = 107,
        CommandUnavailable = 108,
        NotInitialized = 109,
        ObjectTooLarge = 116,
        ExceededConfigParamsError = 116,
        InvalidLimitError = 117,
        InvalidSkipError = 118,
        OperationForbidden = 119,
        CacheMiss = 120,
        InvalidNestedKey = 121,
        InvalidFileName = 122,
        InvalidACL = 123,
        RequestTimeout = 124,
        InvalidEmailAddress = 125,
        MissingContentType = 126,
        MissingContentLength = 127,
        InvalidContentLength = 128,
        FileTooLarge = 129,
        FileSaveError = 130,
        FileDeleteError = 131,
        InvalidInstallationIdError = 132,
        InvalidDeviceTypeError = 133,
        InvalidChannelsArrayError = 134,
        MissingRequiredFieldError = 135,
        ChangedImmutableFieldError = 136,
        DuplicateValue = 137,
        InvalidRoleName = 139,
        ReservedValue = 139,
        ExceededCollectionQuota = 140,
        ScriptFailed = 141,
        FunctionNotFound = 141,
        JobNotFound = 141,
        SuccessErrorNotCalled = 141,
        MultupleSuccessErrorCalls = 141,
        ValidationFailed = 142,
        WebhookError = 143,
        InvalidImageData = 150,
        UnsavedFileError = 151,
        InvalidPushTimeError = 152,
        InefficientQueryError = 154,
        RequestLimitExceeded = 155,
        TemporaryRejectionError = 159,
        HostingError = 158,
        InvalidEventName = 160,
        UsernameMissing = 200,
        PasswordMissing = 201,
        UsernameTaken = 202,
        UserEmailTaken = 203,
        UserEmailMissing = 204,
        UserWithEmailNotFound = 205,
        SessionMissing = 206,
        MustCreateUserThroughSignup = 207,
        AccountAlreadyLinked = 208,
        InvalidSessionToken = 209,
        ClassNotEmpty = 255,
        AppNameInvalid = 256,
        DatabaseNotMigratedError = 428,
        AggregateError = 600,
        FileReadError = 601,
        XDomainRequest = 602,
        MissingAPIKeyError = 902,
        InvalidAPIKeyError = 903
    };
}